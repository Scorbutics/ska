#pragma once
#include <vector>

#include "Container/sorted_observable.h"

#include "NodeValue/Token.h"
#include "Service/ExpressionParser.h"

#include "Event/ForTokenEvent.h"
#include "Event/IfElseTokenEvent.h"
#include "Event/VarTokenEvent.h"
#include "Event/BlockTokenEvent.h"
#include "Event/FunctionTokenEvent.h"
#include "Event/ExpressionTokenEvent.h"
#include "Event/ReturnTokenEvent.h"
#include "Event/ArrayTokenEvent.h"
#include "Event/ImportTokenEvent.h"
#include "Event/ScriptLinkTokenEvent.h"
#include "Event/FilterTokenEvent.h"
#include "Event/ConverterTokenEvent.h"

#include "Matcher/MatcherBlock.h"
#include "Matcher/MatcherFor.h"
#include "Matcher/MatcherIfElse.h"
#include "Matcher/MatcherVar.h"
#include "Matcher/MatcherReturn.h"
#include "Matcher/MatcherFilter.h"
#include "Matcher/MatcherConverter.h"
#include "Matcher/MatcherType.h"

#include "NodeValue/ScriptASTPtr.h"
#include "NodeValue/ScriptCacheAST.h"

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class ExpressionParser;
	struct ScriptHandleAST;

	class StatementParser :
		public observable_priority_queue<ForTokenEvent>,
		public observable_priority_queue<IfElseTokenEvent>,
		public observable_priority_queue<VarTokenEvent>,
		public observable_priority_queue<FunctionTokenEvent>,
		public observable_priority_queue<BlockTokenEvent>,
		public observable_priority_queue<ExpressionTokenEvent>,
		public observable_priority_queue<ReturnTokenEvent>,
		public observable_priority_queue<ArrayTokenEvent>,
		public observable_priority_queue<ImportTokenEvent>,
		public observable_priority_queue<ScriptLinkTokenEvent>,
		public observable_priority_queue<FilterTokenEvent>,
		public observable_priority_queue<ConverterTokenEvent> {

		using ASTNodePtr = std::unique_ptr<ska::ASTNode>;
		friend class ScriptAST;
		
		using ScriptHandleASTPtr = std::unique_ptr<ScriptHandleAST>;
	public:
		StatementParser(const ReservedKeywordsPool& reservedKeywordsPool);
		ScriptASTPtr subParse(ScriptCacheAST& scriptCache, const std::string& name, std::ifstream& file);

	private:
		ASTNodePtr parse(ScriptAST& input);

		ASTNodePtr statement(ScriptAST& input);
    	ASTNodePtr optstatement(ScriptAST& input, const Token& mustNotBe = Token{});

		ASTNodePtr expr(ScriptAST& input);
		ASTNodePtr optexpr(ScriptAST& input, const Token& mustNotBe = Token{});

		ASTNodePtr matchExpressionStatement(ScriptAST& input);
		ASTNodePtr matchReservedKeyword(ScriptAST& input, const std::size_t keywordIndex);
		static void error(const std::string& message);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		ExpressionParser m_expressionParser;
		
		MatcherBlock m_matcherBlock;
		MatcherFor m_matcherFor;
		MatcherVar m_matcherVar;
		MatcherIfElse m_matcherIfElse;
		MatcherType m_matcherType;
		MatcherConverter m_matcherConverter;
		MatcherReturn m_matcherReturn;
		MatcherImport m_matcherImport;
		MatcherFilter m_matcherFilter;
	};
}
