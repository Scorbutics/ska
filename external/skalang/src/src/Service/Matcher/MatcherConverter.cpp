#include "Config/LoggerConfigLang.h"
#include "MatcherConverter.h"
#include "MatcherType.h"

#include "NodeValue/AST.h"
#include "Service/TokenReader.h"
#include "Service/StatementParser.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Event/ConverterTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherConverter)

ska::ASTNodePtr ska::MatcherConverter::match(ScriptAST& input) {
	SLOG(ska::LogLevel::Debug) << "converter declaration";

	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());

	std::string typeAsStr = ":";
	auto converterTypeNode = m_matcherType.match(input.reader(), &typeAsStr);
	SLOG(ska::LogLevel::Info) << "converter detected type : \"" << typeAsStr << "\"";

	auto functionName = Token{ std::move(typeAsStr), TokenType::IDENTIFIER, converterTypeNode->positionInScript() };

	auto emptyNode = ASTFactory::MakeEmptyNode();
	auto startEvent = FunctionTokenEvent{ *emptyNode, FunctionTokenEventType::DECLARATION_NAME, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(startEvent);

	auto prototypeNode = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionName, std::move(converterTypeNode));

	auto functionEvent = VarTokenEvent::MakeFunction(*prototypeNode, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(functionEvent);

	SLOG(ska::LogLevel::Debug) << "reading converter body";
	auto functionBodyNode = matchConverterBody(input);
	SLOG(ska::LogLevel::Debug) << "converter read.";

	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(prototypeNode), std::move(functionBodyNode));

	auto statementEvent = FunctionTokenEvent{ *functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(statementEvent);

	return std::move(functionDeclarationNode);
}

ska::ASTNodePtr ska::MatcherConverter::matchConverterBody(ScriptAST& input) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());

	auto statements = std::vector<ASTNodePtr>{};
	while (!input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
		auto optionalStatement = input.statement(m_parser);
		if (optionalStatement != nullptr && !optionalStatement->logicalEmpty()) {
			statements.push_back(std::move(optionalStatement));
		}
	}
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

	return ASTFactory::MakeNode<Operator::BLOCK>(std::move(statements));
}

ska::ASTNodePtr ska::MatcherConverter::matchCall(ScriptAST& input, ASTNodePtr objectConvertedNode) {
	std::string converterName = ":";
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
	auto typeNode = m_matcherType.match(input.reader(), &converterName);

	if (OperatorTraits::isNamed(objectConvertedNode->op())) {
		auto objectEvent = VarTokenEvent::MakeUse(*objectConvertedNode, input);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(objectEvent);
	}

	auto event = ExpressionTokenEvent{ *objectConvertedNode, input };
	m_parser.observable_priority_queue<ExpressionTokenEvent>::notifyObservers(event);

	if (objectConvertedNode->type() == ExpressionType::OBJECT) {
		// User defined converter
		auto converterNameNode = ASTFactory::MakeLogicalNode(Token{ converterName, TokenType::IDENTIFIER, typeNode->positionInScript() });
		auto converterFieldAccess = ASTFactory::MakeNode<Operator::FIELD_ACCESS>(std::move(objectConvertedNode), std::move(converterNameNode));
		auto converterCallNode = ASTFactory::MakeNode<Operator::FUNCTION_CALL>(std::move(converterFieldAccess));

		auto callEvent = FunctionTokenEvent{ *converterCallNode, FunctionTokenEventType::CALL, input, std::move(converterName) };
		m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(callEvent);

		return converterCallNode;
	}

	//Native built-in types converter
	auto converterNode = ASTFactory::MakeNode<Operator::CONVERTER_CALL>(std::move(objectConvertedNode), std::move(typeNode));
	
	auto converterEvent = ConverterTokenEvent{ *converterNode, input };
	m_parser.observable_priority_queue<ConverterTokenEvent>::notifyObservers(converterEvent);

	return converterNode;
}
