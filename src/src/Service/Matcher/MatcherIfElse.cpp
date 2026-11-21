#include "Config/LoggerConfigLang.h"
#include "MatcherIfElse.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherIfElse)

ska::ASTNodePtr ska::MatcherIfElse::match(ScriptAST& input) {
    return matchIf(input, true);
}

// "else if" : the else block only contains the next "if", which shares the "end" of the first one
ska::ASTNodePtr ska::MatcherIfElse::matchElseIf(ScriptAST& input) {
    auto blockNode = ASTFactory::MakeNode<Operator::BLOCK>();
    auto startEvent = BlockTokenEvent{ *blockNode, BlockTokenEventType::START };
    m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(startEvent);

    auto statements = std::vector<ASTNodePtr>{};
    statements.push_back(matchIf(input, false));
    blockNode = ASTFactory::MakeNode<Operator::BLOCK>(std::move(statements));

    auto endEvent = BlockTokenEvent{ *blockNode, BlockTokenEventType::END };
    m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(endEvent);
    return blockNode;
}

ska::ASTNodePtr ska::MatcherIfElse::matchIf(ScriptAST& input, bool matchEnd) {
    auto ifNode = ASTNodePtr{};

    SLOG(ska::LogLevel::Debug) << "Matching if";

    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::IF>());

    {
        auto conditionExpression = input.expr(m_parser);
        SLOG(ska::LogLevel::Debug) << "Matching if - expression done";
        // Optional "do" after the condition ("if condition do ... end"), as for the other blocks of the language
        if (!input.reader().empty() && input.reader().actual().grammar() == TokenGrammar::BLOCK_BEGIN) {
            input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
        }

        const auto elseToken = m_reservedKeywordsPool.pattern<TokenGrammar::ELSE>();
        const auto endToken = m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>();

        auto conditionStatement = m_matcherBlock.matchNoBoundaries(input, { TokenGrammar::ELSE, TokenGrammar::BLOCK_END });
        SLOG(ska::LogLevel::Debug) << "Matching if - if statement done";

        if (input.reader().expect(elseToken)) {
            SLOG(ska::LogLevel::Debug) << "Matching if - else statement done";
            input.reader().match(elseToken);
            auto elseBlockStatement = input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::IF>()) ?
                matchElseIf(input) :
                m_matcherBlock.matchNoBoundaries(input, { TokenGrammar::BLOCK_END });
            ifNode = ASTFactory::MakeNode<Operator::IF_ELSE>(std::move(conditionExpression), std::move(conditionStatement), std::move(elseBlockStatement));
        } else {
            ifNode = ASTFactory::MakeNode<Operator::IF>(std::move(conditionExpression), std::move(conditionStatement));
        }
        if (matchEnd) {
            input.reader().match(endToken);
        }
    }
    SLOG(ska::LogLevel::Debug) << "if matching done";
    auto event = IfElseTokenEvent {*ifNode};
	m_parser.observable_priority_queue<IfElseTokenEvent>::notifyObservers(event);
    return ifNode;
}
