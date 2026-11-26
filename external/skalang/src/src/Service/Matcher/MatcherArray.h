#pragma once
#include <memory>
#include <string>

#include "Container/expression_stack.h"
#include "NodeValue/Token.h"

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;
	class MatcherType;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;

	class MatcherArray {
		using ExpressionStack = expression_stack<Token, ASTNodePtr>;
	public:
		~MatcherArray() = default;
		MatcherArray(const ReservedKeywordsPool& pool, StatementParser& parser, MatcherType& matcherType) :
			m_reservedKeywordsPool(pool), m_parser(parser), m_matcherType(matcherType) {}
		
		[[nodiscard]]
		ASTNodePtr match(ScriptAST& input, ExpressionStack& operands, char token, bool isDoingOperation);
	
	private:
		ASTNodePtr matchUse(ScriptAST& input, ASTNodePtr identifierArrayAffected);
		ASTNodePtr matchDeclaration(ScriptAST& input);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
		MatcherType& m_matcherType;
	};
}
