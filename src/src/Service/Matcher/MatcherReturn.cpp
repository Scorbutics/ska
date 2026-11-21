#include "Config/LoggerConfigLang.h"
#include "MatcherReturn.h"
#include "MatcherConverter.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherReturn)

ska::ASTNodePtr ska::MatcherReturn::match(ScriptAST& input) {
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::RETURN>());

    auto returnNode = ASTNodePtr {};

	auto returnStartEvent = ReturnTokenEvent{ input };
	m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnStartEvent);


    if (input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_BEGIN>())) {
        returnNode = matchCustomObject(input);
    } else {
        returnNode = matchBuiltIn(input);
    }
    input.reader().mightMatch(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

    return returnNode;
}

ska::ASTNodePtr ska::MatcherReturn::matchCustomObject(ScriptAST& input) {
    auto returnFieldNodes = std::vector<ASTNodePtr>{};
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_BEGIN>());
    while (!input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_END>())) {
        input.reader().mightMatch(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

        ASTNodePtr fieldNode;
        if (input.reader().expect(TokenType::IDENTIFIER)) {
            fieldNode = matchField(input);
        } else if (input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>())) {
            fieldNode = m_matcherConverter.match(input);
        }

        returnFieldNodes.push_back(std::move(fieldNode));

        if (input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>())) {
            input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
        } else if (!input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_END>())) {
            throw std::runtime_error("an object block-end token was expected");
        }
    }

    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_END>());

    //auto definedNode = ASTFactory::MakeLogicalNode(Token { input.symbols().enclosingType()->name(), TokenType::IDENTIFIER, {} });
    auto returnNode = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(returnFieldNodes)));

	auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT>(*returnNode, input);
	m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);

    return returnNode;
}

ska::ASTNodePtr ska::MatcherReturn::matchBuiltIn(ScriptAST& input) {
    // "return" alone (end of the statement) : leaves a function without returned value
    const auto noValue = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>()) ||
        input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
    auto value = noValue ? ASTFactory::MakeEmptyNode() : input.expr(m_parser);
    auto node = ASTFactory::MakeNode<Operator::RETURN>(std::move(value));

	auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::BUILTIN>(*node, input);
	m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);

    return node;
}

ska::ASTNodePtr ska::MatcherReturn::matchField(ScriptAST& input) {
    auto field = input.reader().match(TokenType::IDENTIFIER);

    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());
    // A function declared as the value of a field takes its name, the other ones are anonymous
    const auto namesFunction = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::FUNCTION>());
    input.pushContext({ namesFunction ? ParsingContextType::AFFECTATION : ParsingContextType::VALUE, field });
    auto fieldValue = input.expr(m_parser);

    input.popContext();

    SLOG(ska::LogLevel::Info) << "Constructor with field \"" << field << "\" and field value \"" << (*fieldValue) << "\"";

    auto node = ASTFactory::MakeNode<Operator::DECLARATION>(std::move(field), std::move(fieldValue));

    auto event = VarTokenEvent::template Make<VarTokenEventType::DECLARATION>(*node, input);
    m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);

    return node;
}
