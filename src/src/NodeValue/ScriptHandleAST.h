#pragma once
#include <vector>
#include <memory>
#include "ASTNodePtr.h"
#include "Service/SymbolTable.h"
#include "Service/TokenReader.h"
#include "ParsingContext.h"
#include "Token.h"

namespace ska {
	class ScriptAST;
	struct ScriptCacheAST;
	struct ScriptHandleAST {

	public:
		ScriptHandleAST(const ScriptHandleAST&) = delete;
		ScriptHandleAST& operator=(const ScriptHandleAST&) = delete;

		const SymbolTable& symbols() const { return m_symbols; }
		SymbolTable& symbols() { return m_symbols; }

		bool hasRootNode() const { return m_ast != nullptr; }
		ASTNode& rootNode() { assert(m_ast != nullptr); return *m_ast; }
		const ASTNode& rootNode() const { assert(m_ast != nullptr); return *m_ast; }

		const std::string& name() const { return m_name; }

		std::size_t id() const { return m_id; }

	private:
		ScriptHandleAST(ScriptCacheAST& cache, std::vector<Token> input, std::size_t startIndex = 0, std::string name = "");
		friend class ScriptAST;

		std::string m_name;
		ScriptCacheAST& m_cache;
		ParsingContextStack m_parsingContexts;
		TokenReader m_input;
		SymbolTable m_symbols;
		ASTNodePtr m_ast;
		bool m_bridged = false;
		std::size_t m_id = -1;
	};

	using ScriptHandleASTPtr = std::unique_ptr<ScriptHandleAST>;
}
