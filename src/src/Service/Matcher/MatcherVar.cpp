#include "Config/LoggerConfigLang.h"
#include "MatcherVar.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherVar)

ska::ASTNodePtr ska::MatcherVar::matchDeclaration(ScriptAST& input) {
	SLOG(ska::LogLevel::Info) << "variable declaration";

	auto varNodeIdentifier = input.reader().match(TokenType::IDENTIFIER);
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());

	SLOG(ska::LogLevel::Info) << "equal sign matched, reading expression";

    auto varNodeExpression = input.expr(m_parser);

    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

	SLOG(ska::LogLevel::Info) << "expression end as statement end";

    auto varNode = ASTFactory::MakeNode<Operator::DECLARATION>(std::move(varNodeIdentifier), std::move(varNodeExpression));

    auto event = VarTokenEvent::template Make<VarTokenEventType::DECLARATION> (*varNode, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);

    return varNode;
}

ska::ASTNodePtr ska::MatcherVar::matchAffectation(ScriptAST& input, ASTNodePtr varAffectedNode) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());

	const auto isVariableAffectation = varAffectedNode->size() == 0 && varAffectedNode->tokenType() == TokenType::IDENTIFIER;
	// A function declared as the value of a variable takes its name ("f = function() do ... end"), the other ones are anonymous
	const auto namesFunction = isVariableAffectation && input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::FUNCTION>());
	input.pushContext({ namesFunction ? ParsingContextType::AFFECTATION : ParsingContextType::VALUE, Token{ varAffectedNode->name(), varAffectedNode->tokenType(), varAffectedNode->positionInScript() } });

	auto expressionNode = input.expr(m_parser);
	input.popContext();
	if (expressionNode == nullptr) {
		throw std::runtime_error("syntax error : Affectation incomplete : no expression");
	}

	auto affectationNode = ASTNodePtr{};
	if (isVariableAffectation) {
/*
		if (varAffectedNode->isSymbolicLeaf)
		SLOG(ska::LogLevel::Info) << "\tImplicit field access ! symbol \"" << (objectAccessed->symbol() == nullptr ? "" : objectAccessed->symbol()->name()) << "\"";

		ASTFactory::MakeNode<Operator::FIELD_ACCESS>(std::move(objectAccessed), std::move(fieldAccessed));
*/
		auto nodeName = Token{ varAffectedNode->name(), varAffectedNode->tokenType(), varAffectedNode->positionInScript() };
		affectationNode = ASTFactory::MakeNode<Operator::DECLARATION>(std::move(nodeName), std::move(expressionNode));
		auto event = VarTokenEvent::template Make<VarTokenEventType::DECLARATION>(*affectationNode, input);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	} else {
		affectationNode = ASTFactory::MakeNode<Operator::AFFECTATION>(std::move(varAffectedNode), std::move(expressionNode));
		auto event = VarTokenEvent::template Make<VarTokenEventType::AFFECTATION>(*affectationNode, input);
		m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}

	return affectationNode;
}
