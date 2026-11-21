#include "Config/LoggerConfigLang.h"
#include "Service/LanguageHints.h"
#include "MatcherFunction.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Event/ReturnTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFunction)

ska::ASTNodePtr ska::MatcherFunction::matchDeclaration(ScriptAST& input) {
	SLOG(ska::LogLevel::Debug) << "function declaration";
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::FUNCTION>());

	const auto functionNameContext = input.contextOf(ParsingContextType::AFFECTATION);
	if (functionNameContext == nullptr) {
		throw std::runtime_error("trying to declare a function, but unable to find its affectation name : " + hints::NamedFunction());
	}

	const auto& functionName = *functionNameContext;

	auto emptyNode = ASTFactory::MakeEmptyNode();
	auto startEvent = FunctionTokenEvent{ *emptyNode, FunctionTokenEventType::DECLARATION_NAME, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(startEvent);

	auto parameterListNode = fillDeclarationParameters(input);
	auto returnTypeNode = matchDeclarationReturnType(input);

	if (returnTypeNode->size() > 0 && (*returnTypeNode)[0].name() == m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>().name()) {
		SLOG(ska::LogLevel::Debug) << "factory detected (" << functionName.name() << ")";
		return m_matcherFactory.matchDeclaration(input, functionName, std::move(parameterListNode), std::move(returnTypeNode));
	}

	SLOG(ska::LogLevel::Debug) << "free-function detected (" << functionName.name() << ")";
	return matchClassicFunctionDeclaration(input, functionName, std::move(parameterListNode), std::move(returnTypeNode));
}

ska::ASTNodePtr ska::MatcherFunction::matchClassicFunctionDeclaration(ScriptAST& input, const Token& functionName, std::deque<ASTNodePtr> parameters, ASTNodePtr returnType) {
	if (input.contextOf(ParsingContextType::FACTORY_DECLARATION) != nullptr) {
		SLOG(ska::LogLevel::Debug) << "function \"" << functionName.name() << "\" is a function member";
		input.pushContext({ParsingContextType::FUNCTION_MEMBER_DECLARATION, functionName});
		//parameters.push_front(m_matcherFactory.buildThisObject(input));
	} else {
		input.pushContext({ParsingContextType::FUNCTION_DECLARATION, functionName});
	}

	for (auto& parameter : parameters) {
		auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0], input);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}
	parameters.push_back(std::move(returnType));

	auto prototypeNode = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionName, std::move(parameters));

	auto functionEvent = VarTokenEvent::MakeFunction(*prototypeNode, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(functionEvent);

	SLOG(ska::LogLevel::Debug) << "reading function body";
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
	auto functionBodyNode = ASTFactory::MakeNode<Operator::BLOCK>(matchDeclarationBody(input, m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>()));
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
	SLOG(ska::LogLevel::Debug) << "function read.";

	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(prototypeNode), std::move(functionBodyNode));

	auto statementEvent = FunctionTokenEvent {*functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(statementEvent);

	input.popContext();
	return functionDeclarationNode;
}

std::deque<ska::ASTNodePtr> ska::MatcherFunction::matchParameters(ScriptAST& input) {
	auto functionCallNodeContent = std::deque<ASTNodePtr>{};

	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());
	const auto endParametersToken = m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>();
	const auto endStatementToken = m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>();
	while (!input.reader().expect(endParametersToken)) {

		auto expressionOpt = input.expr(m_parser);
		if (expressionOpt != nullptr) {
			SLOG(ska::LogLevel::Debug) << "Expression not null";
			functionCallNodeContent.push_back(std::move(expressionOpt));
		} else {
			SLOG(ska::LogLevel::Debug) << "Expression null";
			if (input.reader().expect(endStatementToken)) {
				break;
			}
		}

        const auto commaToken = m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>();
		if(input.reader().expect(commaToken)) {
			input.reader().match(commaToken);
		}
	}
	if (input.reader().expect(endParametersToken)) {
		input.reader().match(endParametersToken);
	}
	return functionCallNodeContent;
}

ska::ASTNodePtr ska::MatcherFunction::matchCall(ScriptAST& input, ASTNodePtr identifierFunctionName) {
	auto functionCallNodeContent = matchParameters(input);

	auto functionCallNode = ASTNodePtr{};
	auto functionEventType = FunctionTokenEventType{};

	functionCallNodeContent.push_front(std::move(identifierFunctionName));
	functionCallNode = ASTFactory::MakeNode<Operator::FUNCTION_CALL>(std::move(functionCallNodeContent));
	functionEventType = FunctionTokenEventType::CALL;

	auto event = FunctionTokenEvent { *functionCallNode, functionEventType, input };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(event);

	return functionCallNode;
}


ska::ASTNodePtr ska::MatcherFunction::matchDeclarationParameter(ScriptAST& input) {
	const auto isRightParenthesis = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	if(isRightParenthesis) {
		return nullptr;
	}
	const auto& id = input.reader().match(TokenType::IDENTIFIER);

	const auto& typeDelimiterToken = m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>();
    input.reader().match(typeDelimiterToken);

	auto typeNameNode = m_matcherType.match(input.reader());

	SLOG(ska::LogLevel::Debug) << id.name();
	return ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(id, std::move(typeNameNode));
}

std::deque<ska::ASTNodePtr> ska::MatcherFunction::fillDeclarationParameters(ScriptAST& input) {
	if (input.reader().expect(TokenType::IDENTIFIER)) {
		throw std::runtime_error("a name after \"function\" : " + hints::NamedFunction());
	}
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto parameters = std::deque<ASTNodePtr>{};
	auto isRightParenthesis = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	auto isComma = true;
	while (!isRightParenthesis && isComma) {
		if (!input.reader().expect(TokenType::SYMBOL)) {
			SLOG(ska::LogLevel::Debug) << "parameter detected, reading identifier : ";
			auto parameterNode = matchDeclarationParameter(input);
			parameters.push_back(std::move(parameterNode));
			isComma = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
		isRightParenthesis = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	}
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

	return parameters;
}

ska::ASTNodePtr ska::MatcherFunction::matchDeclarationReturnType(ScriptAST& input) {
	if (input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>())) {
		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
		auto typeNode = m_matcherType.match(input.reader());
		SLOG(ska::LogLevel::Debug) << "function type detected : " << typeNode->name();
		return typeNode;
	}

	SLOG(ska::LogLevel::Debug) << "void function detected";
	return ASTFactory::MakeLogicalNode(ska::Token{ "", ska::TokenType::IDENTIFIER, input.reader().actual().position() });
}

std::vector<ska::ASTNodePtr> ska::MatcherFunction::matchDeclarationBody(ScriptAST& input, const Token& until) {
	auto statements = std::vector<ASTNodePtr>{};
	while (!input.reader().expect(until)) {
		auto optionalStatement = input.statement(m_parser);
		if (optionalStatement != nullptr && !optionalStatement->logicalEmpty()) {
			statements.push_back(std::move(optionalStatement));
		}
	}
	return std::move(statements);
}
