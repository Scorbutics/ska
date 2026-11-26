#include <iostream>
#include "Config/LoggerConfigLang.h"
#include "Service/LanguageHints.h"
#include "ExpressionParser.h"
#include "Service/StatementParser.h"
#include "NodeValue/AST.h"
#include "ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ExpressionParser)

ska::ExpressionParser::ExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, StatementParser& parser) :
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_parser(parser),
	m_matcherType(m_reservedKeywordsPool),
	m_matcherArray(m_reservedKeywordsPool, m_parser, m_matcherType),
	m_matcherFunction(m_reservedKeywordsPool, m_parser),
	m_matcherVar(m_reservedKeywordsPool, m_parser),
	m_matcherImport(m_reservedKeywordsPool, m_parser),
	m_matcherConverter(m_reservedKeywordsPool, m_parser, m_matcherType) {
}

ska::ASTNodePtr ska::ExpressionParser::parse(ScriptAST& input) {
	auto data = expression_stack<Token, ASTNodePtr>{};
	return expression(input, data);
}

ska::ASTNodePtr ska::ExpressionParser::matchVariable(ScriptAST& input, const Token& token) {
	auto varNode = ASTFactory::MakeLogicalNode(input.reader().match(token.type()));
	if (input.reader().actual() == m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>()) {
		return m_matcherVar.matchAffectation(input, std::move(varNode));
	}

	// TODO closure support instead
	/*if (input.contextOf(ParsingContextType::FUNCTION_MEMBER_DECLARATION, 1) != nullptr) {
		return m_matcherFunction.matchPrivateFieldUse(input, std::move(varNode));
	}*/

	auto event = VarTokenEvent::MakeUse(*varNode, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return varNode;
}

std::pair<bool, int> ska::ExpressionParser::parseTokenExpression(ScriptAST& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation) {
	auto rangeCounterOffsetPostMatching = 0;

	switch (token.type()) {
	case TokenType::RESERVED: {
		SLOG(ska::LogLevel::Info) << "\tPushing reserved";
		auto reservedNode = matchReserved(input);
		if (reservedNode != nullptr) {
			expressions.push(std::move(reservedNode));
			break;
		}
	}
	case TokenType::BOOLEAN:
	case TokenType::STRING:
	case TokenType::DIGIT:
		SLOG(ska::LogLevel::Info) << "\tPushing operand " << token;
		expressions.push(ASTFactory::MakeLogicalNode(input.reader().match(token.type())));
		break;

	case TokenType::IDENTIFIER: {
		SLOG(ska::LogLevel::Info) << "\tPushing var operand " << token;
		expressions.push(matchVariable(input, token));
	} break;

	case TokenType::ARRAY: {
		const auto& value = std::get<std::string>(token.content());
		expressions.push(m_matcherArray.match(input, expressions, value[0], isDoingOperation));
	} break;

	case TokenType::RANGE:
		rangeCounterOffsetPostMatching = matchRange(input, expressions, token, isDoingOperation);
		break;

	case TokenType::DOT_SYMBOL: {
		//Field access only (digits are DIGIT token type, even real ones)
		auto expressionObject = expressions.popOperandIfNoOperator(isDoingOperation);
		if (expressionObject == nullptr) {
			throw std::runtime_error("invalid operator \".\" placement");
		}
		expressions.push(matchObjectFieldAccess(input, std::move(expressionObject)));
	} break;


	case TokenType::SYMBOL:
		return { matchSymbol(input, expressions, token, isDoingOperation), rangeCounterOffsetPostMatching };

	default:
		error("Expected a symbol, a literal, an identifier or a reserved keyword, but got the token \"" + token.name() + "\"");
	}

	return { false, rangeCounterOffsetPostMatching };
}

int ska::ExpressionParser::matchRange(ScriptAST& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation) {
	const auto& value = std::get<std::string>(token.content());
  switch (value[0]) {
	case '(':
		return matchParenthesis(input, expressions, isDoingOperation);

	case ')': {
		expressions.push(Token{ input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()) });

		auto endGrouping = false;
		auto groupParenthesisRange = [&endGrouping](const Token& t) {
			if(endGrouping) {
				return Group::FlowControl::STOP;
			}
			const auto& strValue = t.name();
			if (t.type() == TokenType::RANGE) {
				endGrouping = strValue == "(";
				return Group::FlowControl::IGNORE_AND_CONTINUE;
			}
			return Group::FlowControl::GROUP;
		};

		auto result = expressions.groupAndPop<ASTFactory>(std::move(groupParenthesisRange));
		if (result != nullptr) {
			expressions.push(std::move(result));
		}
		SLOG(ska::LogLevel::Info) << "\tRange end";
	} return 0;

	default:
    	error("Unexpected token (range type) \"" + token.name() + "\"" + (token.name() == "{" ? hints::BraceBlock() : ""));
		return 0;
	}
}

bool ska::ExpressionParser::matchSymbol(ScriptAST& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation) {
	const auto& value = std::get<std::string>(token.content());
	// TODO Pattern matching

	ASTNodePtr toPush;
	if (value == "=") {
		//We must check that the token before the '=' is an lvalue : done in the semantic check pass.
		expressions.push(m_matcherVar.matchAffectation(input, expressions.popOperandIfNoOperator(isDoingOperation)));
		return false;
	}

	if (value == ":") {
		auto expressionObject = expressions.popOperandIfNoOperator(isDoingOperation);
		if (expressionObject == nullptr) {
			throw std::runtime_error("invalid converter placement");
		}
		expressions.push(m_matcherConverter.matchCall(input, std::move(expressionObject)));
		return false;
	}

	if (value == "-" && expressions.expectsOperand()) {
		const auto minus = input.reader().match(TokenType::SYMBOL);
		if (input.reader().expect(TokenType::DIGIT)) {
			// Negative number
			const auto number = input.reader().match(TokenType::DIGIT);
			expressions.push(ASTFactory::MakeLogicalNode(Token{ "-" + std::get<std::string>(number.content()), TokenType::DIGIT, minus.position() }));
			return false;
		}
		// Negation of an operand : "0 - operand", the subtraction binding tighter than the other operators
		SLOG(ska::LogLevel::Info) << "\t\tPushing negation";
		expressions.push(ASTFactory::MakeLogicalNode(Token{ "0", TokenType::DIGIT, minus.position() }));
		expressions.push(Token{ ExpressionStack::NegationOperator, TokenType::SYMBOL, minus.position() });
		return true;
	}

	if (value == "!" && expressions.expectsOperand()) {
		// Logical not : "false == operand", the comparison binding tighter than the other operators
		const auto notSymbol = input.reader().match(TokenType::SYMBOL);
		expressions.push(ASTFactory::MakeLogicalNode(Token{ "false", TokenType::BOOLEAN, notSymbol.position() }));
		expressions.push(Token{ ExpressionStack::NotOperator, TokenType::SYMBOL, notSymbol.position() });
		return true;
	}

	SLOG(ska::LogLevel::Info) << "\t\tPushing operator symbol " << value;

	const auto shouldPopOperatorsStack = expressions.checkLessPriorityOperator(value);
	if (shouldPopOperatorsStack) {
		SLOG(ska::LogLevel::Debug) << "\tLess precedence, poping first (top) operator before adding " << value;

		auto poped = false;
		auto expressionGroup = expressions.groupAndPop<ASTFactory>(Group::FirstOnly<Token>(poped));
		if (expressionGroup == nullptr) {
			error("void expression on affectation");
		}
		expressions.push(std::move(expressionGroup));
	} else {
		expressions.push(Token{ input.reader().match(TokenType::SYMBOL) });
	}

	return true;
}

int ska::ExpressionParser::matchParenthesis(ScriptAST& input, ExpressionStack& expressions, bool isDoingOperation) {
	// A parenthesis where an operand is expected is a grouping one, not a function call on the previous operand (ex : "((a))")
	auto functionNameOperand = expressions.expectsOperand() ? nullptr : expressions.popOperandIfNoOperator(isDoingOperation);
	if (functionNameOperand != nullptr) {
		auto event = ExpressionTokenEvent{ *functionNameOperand, input };
		m_parser.observable_priority_queue<ExpressionTokenEvent>::notifyObservers(event);

		if (functionNameOperand->type() == ExpressionType::FUNCTION) {
			SLOG(ska::LogLevel::Info) << "\tFunction call !";
			//We already pushed the identifier as an operand, but in fact it's a function call.
			//We have to pop it, then matching the function call as the new operand.
			expressions.push(m_matcherFunction.matchCall(input, std::move(functionNameOperand)));
			return -1;
		} else if (functionNameOperand->type().has_value()) {
			auto ss = std::stringstream{};
			ss << "expected a function as identifier but got a \"" << functionNameOperand->type().value() << "\"";
			error(ss.str());
		}
	}

	SLOG(ska::LogLevel::Info) << "\tRange begin";
	expressions.push(Token{ input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()) });
	return 0;
}

ska::ASTNodePtr ska::ExpressionParser::matchObjectFieldAccess(ScriptAST& input, ASTNodePtr objectAccessed) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::FIELD_ACCESS_OPERATOR>());
	auto fieldAccessedIdentifier = input.reader().match(TokenType::IDENTIFIER);
	auto fieldAccessed = ASTFactory::MakeLogicalNode(fieldAccessedIdentifier);

	if (OperatorTraits::isNamed(objectAccessed->op())) {
		auto objectEvent = VarTokenEvent::MakeUse(*objectAccessed, input);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(objectEvent);
	}

	SLOG(ska::LogLevel::Info) << "\tField access ! symbol \"" << (objectAccessed->symbol() == nullptr ? "" : objectAccessed->symbol()->name()) << "\"";

	return ASTFactory::MakeNode<Operator::FIELD_ACCESS>(std::move(objectAccessed), std::move(fieldAccessed));
}

bool ska::ExpressionParser::isAtEndOfExpression(ScriptAST& input) const {
	return input.reader().expectOneType({
		TokenGrammar::STATEMENT_END,
		TokenGrammar::ARGUMENT_DELIMITER,
		TokenGrammar::BLOCK_BEGIN,
		TokenGrammar::BLOCK_END,
		TokenGrammar::OBJECT_BLOCK_END,
		TokenGrammar::FILTER,
		TokenGrammar::BRACKET_END
	});
}

ska::ASTNodePtr ska::ExpressionParser::expression(ScriptAST& input, ExpressionStack& expressions) {
	auto rangeCounter = 0;
	auto isDoingOperation = false;
	while (!isAtEndOfExpression(input) && rangeCounter >= 0) {
		SLOG(ska::LogLevel::Debug) << "\tRange counter " << rangeCounter;

		rangeCounter += input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()) ? 1 : 0;
		rangeCounter -= input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()) ? 1 : 0;

		auto token = input.reader().actual();
		if (rangeCounter >= 0) {
			auto rangeCounterOffsetPostMatching = 0;
			std::tie(isDoingOperation, rangeCounterOffsetPostMatching) = parseTokenExpression(input, expressions, token, isDoingOperation);
			rangeCounter += rangeCounterOffsetPostMatching;
		}
	}
	SLOG(ska::LogLevel::Debug) << "\tPoping everything";

	auto expressionGroup = expressions.template groupAndPop <ASTFactory>(Group::All<Token>);
	if (expressionGroup != nullptr) {
		auto event = ExpressionTokenEvent { *expressionGroup, input };
		m_parser.observable_priority_queue<ExpressionTokenEvent>::notifyObservers(event);
	}

	return expressionGroup;
}

ska::ASTNodePtr ska::ExpressionParser::matchReserved(ScriptAST& input) {
	const auto& result = input.reader().actual();

	switch(std::get<std::size_t>(result.content())) {
		case static_cast<std::size_t>(TokenGrammar::FUNCTION):
			return m_matcherFunction.matchDeclaration(input);
		case static_cast<std::size_t>(TokenGrammar::IMPORT) :
			return m_matcherImport.matchImport(input);
		default:
			return nullptr;
	}
}

void ska::ExpressionParser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}



