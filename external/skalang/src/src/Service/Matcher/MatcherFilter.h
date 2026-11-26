#pragma once
#include <memory>
#include <string>

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;
	class MatcherBlock;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherFilter {
	public:
		~MatcherFilter() = default;
		MatcherFilter(const ReservedKeywordsPool& pool, StatementParser& parser, MatcherBlock& matcherBlock) :
			m_reservedKeywordsPool(pool), m_parser(parser), m_matcherBlock(matcherBlock) {}
	
		ASTNodePtr match(ScriptAST& input, ASTNodePtr collectionToFilter);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
		MatcherBlock& m_matcherBlock;
	};
}