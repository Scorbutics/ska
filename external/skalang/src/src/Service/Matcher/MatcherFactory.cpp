#include "Config/LoggerConfigLang.h"
#include "MatcherFactory.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"
#include "Event/ReturnTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFactory)

std::vector<ska::ASTNodePtr> ska::MatcherFactory::matchDeclarationBody(ScriptAST& input, const Token& endToken) {
    auto statements = std::vector<ASTNodePtr>{};
	while (!input.reader().expect(endToken)) {
		auto optionalStatement = input.statement(m_parser);
		if (optionalStatement != nullptr && !optionalStatement->logicalEmpty()) {
			statements.push_back(std::move(optionalStatement));
		}
	}
    return statements;
}

ska::ASTNodePtr ska::MatcherFactory::matchDeclaration(ScriptAST& input, const Token& functionName, std::deque<ASTNodePtr> parameters, ASTNodePtr returnType) {
	input.pushContext({ParsingContextType::FACTORY_DECLARATION, functionName});

	SLOG(ska::LogLevel::Debug) << "factory \"" << functionName << "\" matching declaration and parameters part";
	// Match the function name declaration
	// e.g. function(i: int): var
	for (auto& parameter : parameters) {
		auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0], input);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}
	// add the return type to parameter list
	parameters.push_back(std::move(returnType));
	auto prototypeNode = ASTFactory::MakeNode<Operator::FACTORY_PROTOTYPE_DECLARATION>(functionName, std::move(parameters));

	auto functionEvent = VarTokenEvent::MakeFunction(*prototypeNode, input);
	// Declares also the function as a variable
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(functionEvent);

	// Match the factory function body until the "return" token
	// e.g. any statement following "function(i: int): var"
	SLOG(ska::LogLevel::Debug) << "factory matching body part";
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
	auto bodyNodes = matchDeclarationBody(input, m_reservedKeywordsPool.pattern<TokenGrammar::RETURN>());

	// Match the return part of the factory, where the object is built
	// e.g. return { toto = 1 }
	SLOG(ska::LogLevel::Debug) << "factory matching generated object part";
	auto returnNode = m_matcherReturn.match(input);

	// Append return node to the body
	bodyNodes.push_back(std::move(returnNode));

	auto functionBodyNode = ASTFactory::MakeNode<Operator::BLOCK>(std::move(bodyNodes));
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

	auto factoryNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(prototypeNode), std::move(functionBodyNode));

	auto prototypeFactoryEvent = FunctionTokenEvent{ *factoryNode, FunctionTokenEventType::FACTORY_DECLARATION_STATEMENT, input, functionName.name() };
	m_parser.observable_priority_queue<FunctionTokenEvent>::notifyObservers(prototypeFactoryEvent);

	input.popContext();
	return factoryNode;
}
