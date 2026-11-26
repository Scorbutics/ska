#pragma once
#include <memory>
#include <string>
#include <unordered_set>

#include "NodeValue/TokenGrammar.h"

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherBlock {
	public:
		~MatcherBlock() = default;
		MatcherBlock(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr match(ScriptAST& input);
		ASTNodePtr matchNoBoundaries(ScriptAST& input, std::unordered_set<TokenGrammar> stopToken);
	
	private:
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}
