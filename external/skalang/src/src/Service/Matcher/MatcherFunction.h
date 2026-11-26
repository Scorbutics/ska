#pragma once
#include <memory>
#include <string>

#include "MatcherType.h"
#include "MatcherReturn.h"
#include "MatcherConverter.h"
#include "MatcherFactory.h"

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;
	struct Token;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;

	class MatcherFunction {
	public:
		~MatcherFunction() = default;
		MatcherFunction(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser), m_matcherType(pool),
			m_matcherConverter(pool, parser, m_matcherType), m_matcherReturn(pool, parser, m_matcherConverter),
			m_matcherFactory(pool, parser) {}

		ASTNodePtr matchCall(ScriptAST& input, ASTNodePtr identifierFunctionName);
		ASTNodePtr matchDeclaration(ScriptAST& input);

	private:
		ASTNodePtr matchClassicFunctionDeclaration(ScriptAST& input, const Token& functionName, std::deque<ASTNodePtr> parameters, ASTNodePtr returnType);

		std::vector<ASTNodePtr> matchDeclarationBody(ScriptAST& input, const Token& until);
		std::deque<ASTNodePtr> fillDeclarationParameters(ScriptAST& input);
		ASTNodePtr matchDeclarationReturnType(ScriptAST& input);
		ASTNodePtr matchDeclarationParameter(ScriptAST& input);
		std::deque<ASTNodePtr> matchParameters(ScriptAST& input);

		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
		MatcherType m_matcherType;
		MatcherConverter m_matcherConverter;
		MatcherReturn m_matcherReturn;
		MatcherFactory m_matcherFactory;
	};
}
