#include <iostream>
#include "Config/LoggerConfigLang.h"
#include "Service/LanguageHints.h"
#include "ExpressionParser.h"
#include "Service/StatementParser.h"
#include "NodeValue/AST.h"
#include "ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Service/SymbolTable.h"
#include "Service/ScopedSymbolTable.h"

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

ska::ASTNodePtr ska::ExpressionParser::parseMapKey(ScriptAST& input) {
	auto data = expression_stack<Token, ASTNodePtr>{};
	return expression(input, data, true);
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
		TokenGrammar::BRACKET_END,
		TokenGrammar::THEN
	});
}

ska::ASTNodePtr ska::ExpressionParser::expression(ScriptAST& input, ExpressionStack& expressions, bool stopAtTypeDelimiter) {
	auto rangeCounter = 0;
	auto isDoingOperation = false;
	const auto& typeDelimiter = m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>();
	while (!isAtEndOfExpression(input) && rangeCounter >= 0 &&
		!(stopAtTypeDelimiter && rangeCounter == 0 && input.reader().expect(typeDelimiter))) {
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
		case static_cast<std::size_t>(TokenGrammar::ENUM):
			return matchEnum(input);
		case static_cast<std::size_t>(TokenGrammar::MATCH):
			return matchMatch(input);
		case static_cast<std::size_t>(TokenGrammar::WHEN):
		case static_cast<std::size_t>(TokenGrammar::THEN):
			error("\"" + std::string{ TokenGrammarSTR[std::get<std::size_t>(result.content())] } + "\" is only used in a match (match value when a, b then x else y end) ; an if is \"if condition\" (optional \"do\"), the statements, then \"end\"");
			return nullptr;
		default:
			return nullptr;
	}
}

void ska::ExpressionParser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}




void ska::ExpressionParser::skipLineBreaks(ScriptAST& input) {
	const auto& lineBreak = m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>();
	while (input.reader().expect(lineBreak)) {
		input.reader().match(lineBreak);
	}
}

// Status = enum none, poison, burn end : the values are separated by commas or line breaks.
// The enum is a symbol table named after its variable (as a function), its values are its children : a value is
// "Status.poison", of the type "Status" (ENUM, with the symbol table of the enum). At runtime, a value is its name.
ska::ASTNodePtr ska::ExpressionParser::matchEnum(ScriptAST& input) {
	const auto enumToken = input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::ENUM>());
	const auto* context = input.innermostContext({ ParsingContextType::AFFECTATION, ParsingContextType::VALUE,
		ParsingContextType::FUNCTION_DECLARATION, ParsingContextType::FUNCTION_MEMBER_DECLARATION, ParsingContextType::FACTORY_DECLARATION });
	if (context == nullptr || context->type != ParsingContextType::AFFECTATION) {
		error("an enum is declared as the value of a variable : \"Status = enum none, poison end\"");
	}
	const auto enumName = context->owner.name();

	auto emptyNode = ASTFactory::MakeEmptyNode();
	auto startEvent = FunctionTokenEvent{ *emptyNode, FunctionTokenEventType::DECLARATION_NAME, input, enumName };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(startEvent);

	auto& enumTable = input.symbols().current();
	const auto valueType = Type::MakeBuiltIn(ExpressionType::ENUM, &enumTable);

	const auto& endToken = m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>();
	const auto& comma = m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>();
	auto values = std::vector<ASTNodePtr>{};
	skipLineBreaks(input);
	while (!input.reader().expect(endToken)) {
		if (!input.reader().expect(TokenType::IDENTIFIER)) {
			error("the values of an enum are names, separated by commas or line breaks, until \"end\" (\"Status = enum none, poison end\")");
		}
		auto valueToken = input.reader().match(TokenType::IDENTIFIER);
		if (enumTable(valueToken.name()) != nullptr) {
			error("the enum \"" + enumName + "\" has the value \"" + valueToken.name() + "\" twice");
		}
		auto& valueTable = enumTable.createNested(valueToken.name());
		valueTable.changeTypeIfRequired(valueType);
		auto valueNode = ASTFactory::MakeLogicalNode(std::move(valueToken));
		valueNode->updateType(valueType);
		values.push_back(std::move(valueNode));
		if (input.reader().expect(comma)) {
			input.reader().match(comma);
		}
		skipLineBreaks(input);
	}
	input.reader().match(endToken);
	if (values.empty()) {
		error("the enum \"" + enumName + "\" has no value");
	}

	auto enumNode = ASTFactory::MakeNode<Operator::ENUM_DECLARATION>(std::move(values));
	auto endEvent = FunctionTokenEvent{ *enumNode, FunctionTokenEventType::FACTORY_DECLARATION_STATEMENT, input, enumName };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(endEvent);
	return enumNode;
}

// match subject
//     when a, b then x
//     else y
// end
// A value : the result of the first branch with a value equal to the subject, otherwise of "else" (needed, unless the
// branches name every value of an enum). Node : the subject, then a MATCH_BRANCH per branch (the result, then the
// values), the else branch last (a MATCH_BRANCH without value).
ska::ASTNodePtr ska::ExpressionParser::matchMatch(ScriptAST& input) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::MATCH>());
	auto subject = input.expr(m_parser);
	if (subject == nullptr) {
		error("a match needs a value to match (\"match status\" then its branches)");
	}

	const auto& endToken = m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>();
	const auto& whenToken = m_reservedKeywordsPool.pattern<TokenGrammar::WHEN>();
	const auto& thenToken = m_reservedKeywordsPool.pattern<TokenGrammar::THEN>();
	const auto& elseToken = m_reservedKeywordsPool.pattern<TokenGrammar::ELSE>();
	const auto& comma = m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>();

	auto children = std::vector<ASTNodePtr>{};
	children.push_back(std::move(subject));
	auto hasElse = false;
	skipLineBreaks(input);
	while (!input.reader().expect(endToken)) {
		if (hasElse) {
			error("\"else\" is the last branch of a match");
		}
		auto branch = std::vector<ASTNodePtr>{};
		if (input.reader().expect(whenToken)) {
			input.reader().match(whenToken);
			auto values = std::vector<ASTNodePtr>{};
			auto isComma = true;
			while (isComma) {
				auto value = input.expr(m_parser);
				if (value == nullptr) {
					error("a branch of a match is \"when a, b then x\"");
				}
				values.push_back(std::move(value));
				isComma = input.reader().expect(comma);
				if (isComma) {
					input.reader().match(comma);
				}
			}
			if (!input.reader().expect(thenToken)) {
				error("a branch of a match is \"when a, b then x\" (\"then\" is missing)");
			}
			input.reader().match(thenToken);
			auto result = input.expr(m_parser);
			if (result == nullptr) {
				error("a branch of a match gives a value after \"then\"");
			}
			branch.push_back(std::move(result));
			for (auto& value : values) {
				branch.push_back(std::move(value));
			}
		} else if (input.reader().expect(elseToken)) {
			input.reader().match(elseToken);
			auto result = input.expr(m_parser);
			if (result == nullptr) {
				error("the else branch of a match gives a value (\"else x\")");
			}
			branch.push_back(std::move(result));
			hasElse = true;
		} else {
			error("a match has branches \"when a, b then x\", an optional \"else y\", and ends with \"end\"");
		}
		children.push_back(ASTFactory::MakeNode<Operator::MATCH_BRANCH>(std::move(branch)));
		skipLineBreaks(input);
	}
	input.reader().match(endToken);
	if (children.size() == 1) {
		error("a match has at least one branch (\"when a, b then x\")");
	}

	auto matchNode = ASTFactory::MakeNode<Operator::MATCH>(std::move(children));
	auto event = ExpressionTokenEvent{ *matchNode, input };
	m_parser.observable_priority_queue<ExpressionTokenEvent>::notifyObservers(event);
	return matchNode;
}
