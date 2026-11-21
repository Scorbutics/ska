#include "Config/LoggerConfigLang.h"
#include "MatcherFor.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFor)

// "while condition do ... end" : a for loop with only a condition
ska::ASTNodePtr ska::MatcherFor::matchWhile(ScriptAST& input) {
    auto emptyNode = ASTFactory::MakeEmptyNode();
    auto blockEventBegin = BlockTokenEvent { *emptyNode, BlockTokenEventType::START };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(blockEventBegin);
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::WHILE>());
	auto condition = input.expr(m_parser);
	auto statement = input.optstatement(m_parser, Token {});
    auto whileNode = ASTFactory::MakeNode<Operator::FOR_LOOP>(
        ASTFactory::MakeEmptyNode(),
        std::move(condition),
        ASTFactory::MakeEmptyNode(),
        std::move(statement)
    );
    auto event = ForTokenEvent {*whileNode};
	m_parser.observable_priority_queue<ForTokenEvent>::notifyObservers(event);
    auto blockEventEnd = BlockTokenEvent { *emptyNode, BlockTokenEventType::END };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(blockEventEnd);
	return whileNode;
}

ska::ASTNodePtr ska::MatcherFor::match(ScriptAST& input) {
    auto emptyNode = ASTFactory::MakeEmptyNode();
    auto blockEventBegin = BlockTokenEvent { *emptyNode, BlockTokenEventType::START };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(blockEventBegin);

    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::FOR>());
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	SLOG(ska::LogLevel::Info) << "1st for loop expression (= statement)";

	auto forNodeFirstExpression = input.optstatement(m_parser, Token {});

	SLOG(ska::LogLevel::Info) << "2nd for loop expression";

    auto forNodeMidExpression = input.optexpr(m_parser, m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

	SLOG(ska::LogLevel::Info) << "3rd for loop expression";

    auto forNodeLastExpression = input.optexpr(m_parser, m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

	auto forNodeStatement = input.optstatement(m_parser, Token {});
	SLOG(ska::LogLevel::Info) << "end for loop statement";

    auto forNode = ASTFactory::MakeNode<Operator::FOR_LOOP>(
        std::move(forNodeFirstExpression), 
        std::move(forNodeMidExpression), 
        std::move(forNodeLastExpression), 
        std::move(forNodeStatement) 
    );
    
    auto event = ForTokenEvent {*forNode};
	m_parser.observable_priority_queue<ForTokenEvent>::notifyObservers(event);
    
    auto blockEventEnd = BlockTokenEvent { *emptyNode, BlockTokenEventType::END };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(blockEventEnd);
	return forNode;
}
