#include "Config/LoggerConfigLang.h"
#include "MatcherBlock.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherBlock)

ska::ASTNodePtr ska::MatcherBlock::match(ScriptAST& input) {
	if (input.reader().actual().grammar() == TokenGrammar::BLOCK_BEGIN) {
		SLOG(ska::LogLevel::Info) << "block start detected";
		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());

		auto node = matchNoBoundaries(input, { TokenGrammar::BLOCK_END });

		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
		
		SLOG(ska::LogLevel::Info) << "block end";

		return node;
	} else if (input.reader().actual().grammar() == TokenGrammar::BLOCK_END) {
		throw std::runtime_error("syntax error : Block end token encountered when not expected");
	}

	auto expression = input.expr(m_parser);
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
	return expression;
}

ska::ASTNodePtr ska::MatcherBlock::matchNoBoundaries(ScriptAST& input, std::unordered_set<TokenGrammar> stopToken) {
	auto blockNode = ASTFactory::MakeNode<Operator::BLOCK>();
	auto startEvent = BlockTokenEvent{ *blockNode, BlockTokenEventType::START };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(startEvent);

	auto blockNodeStatements = std::vector<ASTNodePtr>{};
	while (!input.reader().expectOneType(stopToken) && !input.reader().empty()) {
		auto optionalStatement = input.optstatement(m_parser, Token{});
		if (!optionalStatement->logicalEmpty()) {
			blockNodeStatements.push_back(std::move(optionalStatement));
		}
	}

	blockNode = ASTFactory::MakeNode<Operator::BLOCK>(std::move(blockNodeStatements));

	auto endEvent = BlockTokenEvent{ *blockNode, BlockTokenEventType::END };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(endEvent);
	
	return blockNode;
}
