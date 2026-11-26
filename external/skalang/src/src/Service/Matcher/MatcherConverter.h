#pragma once
#include <memory>
#include <string>

#include "Container/iterable_stack.h"

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;
	class MatcherType;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherConverter {
	public:
		~MatcherConverter() = default;
		MatcherConverter(const ReservedKeywordsPool& pool, StatementParser& parser, MatcherType& matcherType) :
			m_reservedKeywordsPool(pool), m_parser(parser), m_matcherType(matcherType) {}
	
		ASTNodePtr match(ScriptAST& input);
		ASTNodePtr matchCall(ScriptAST& input, ASTNodePtr objectConvertedNode);

	private:
		ASTNodePtr matchConverterBody(ScriptAST& input);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
		MatcherType& m_matcherType;
	};
}
