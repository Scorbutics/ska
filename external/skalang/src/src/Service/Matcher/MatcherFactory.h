#pragma once
#include <memory>
#include <string>
#include <vector>

#include "MatcherType.h"
#include "MatcherReturn.h"
#include "MatcherConverter.h"

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;
	class Symbol;
	struct Token;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;

	class MatcherFactory {
	public:
        ~MatcherFactory() = default;
		MatcherFactory(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser), m_matcherType(pool),
			m_matcherConverter(pool, parser, m_matcherType), m_matcherReturn(pool, parser, m_matcherConverter) {}

		ASTNodePtr matchDeclaration(ScriptAST& input, const Token& functionName, std::deque<ASTNodePtr> parameters, ASTNodePtr returnType);
    private:
        std::vector<ASTNodePtr> matchDeclarationBody(ScriptAST& input, const Token& endToken);

    	const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
		MatcherType m_matcherType;
		MatcherConverter m_matcherConverter;
		MatcherReturn m_matcherReturn;
    };
}