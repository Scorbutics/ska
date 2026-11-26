#pragma once
#include <memory>
#include <string>

#include "Container/iterable_stack.h"
#include "Container/sorted_observable.h"
#include "Event/BlockTokenEvent.h"
#include "Event/ImportTokenEvent.h"

namespace ska {
	struct ReservedKeywordsPool;
	class ScriptAST;
	class StatementParser;
	struct Token;

	class ASTNode;
	using ASTNodePtr = std::unique_ptr<ASTNode>;
	
	class MatcherImport {
	public:
		~MatcherImport() = default;
		MatcherImport(const ReservedKeywordsPool& pool, StatementParser& parser) :
			m_reservedKeywordsPool(pool), m_parser(parser) {}
	
		ASTNodePtr matchImport(ScriptAST& input);
		
		static ASTNodePtr createNewImport(
			StatementParser& parser,
			observable_priority_queue<ska::BlockTokenEvent>& obsBlock,
			observable_priority_queue<ska::ImportTokenEvent>& obsImport,
			ScriptAST& input,
			Token importNodeClass);

	private:
		ASTNodePtr matchNewImport(ScriptAST& input, Token importNodeClass);
		const ReservedKeywordsPool& m_reservedKeywordsPool;
		StatementParser& m_parser;
	};
}
