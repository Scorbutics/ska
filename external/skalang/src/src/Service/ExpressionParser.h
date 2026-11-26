#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include "Container/expression_stack.h"
#include "NodeValue/ASTNodePtr.h"
#include "TokenReader.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/VarTokenEvent.h"

#include "Matcher/MatcherArray.h"
#include "Matcher/MatcherFunction.h"
#include "Matcher/MatcherVar.h"
#include "Matcher/MatcherImport.h"
#include "Matcher/MatcherType.h"
#include "Matcher/MatcherConverter.h"

namespace ska {
	class ASTNode;
	class StatementParser;
	struct ReservedKeywordsPool;

	class ExpressionParser {

		using PopPredicate = std::function<int(const Token&)>;
		using ExpressionStack = expression_stack<Token, ASTNodePtr>;

	public:
		ExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, StatementParser& parser);
		ASTNodePtr parse(ScriptAST& input);

	private:
		std::pair<bool, int> parseTokenExpression(ScriptAST& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);

		bool matchSymbol(ScriptAST& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);
		int matchRange(ScriptAST& input, ExpressionStack& expressions, const Token& token, bool isDoingOperation);
		int matchParenthesis(ScriptAST& input, ExpressionStack& expressions, bool isDoingOperation);

		ASTNodePtr matchVariable(ScriptAST& input, const Token& token);
		ASTNodePtr matchReserved(ScriptAST& input);
		ASTNodePtr matchObjectFieldAccess(ScriptAST& input, ASTNodePtr objectAccessed);

		bool isAtEndOfExpression(ScriptAST& input) const;

		ASTNodePtr expression(ScriptAST& input, ExpressionStack& expressions);

		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;

		MatcherType m_matcherType;
		MatcherArray m_matcherArray;
		MatcherFunction m_matcherFunction;
		MatcherVar m_matcherVar;
		MatcherImport m_matcherImport;
		MatcherConverter  m_matcherConverter;
	};

}
