#pragma once
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "ScriptASTPtr.h"
#include "ParsingContext.h"
#include "Runtime/Value/NativeFunction.h"

namespace ska {
	class StatementParser;
	class ScriptHandleAST;
	class ScriptCacheAST;
	class ScopedSymbolTable;
	class SymbolTable;
	class TokenReader;

	class ScriptAST {
	public:
		ScriptAST(ScriptHandleAST& handle);

		ScriptAST(ScriptCacheAST& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex = 0, std::size_t scriptId = std::numeric_limits<std::size_t>::max());
		virtual ~ScriptAST() = default;

		bool existsInCache(const std::string& name) const;

		void rewind();

		TokenReader& reader();

		ASTNodePtr statement(StatementParser& parser);
		ASTNodePtr optstatement(StatementParser& parser, const Token& mustNotBe);
		ASTNodePtr expr(StatementParser& parser);
		ASTNodePtr optexpr(StatementParser& parser, const Token& mustNotBe);

		void parse(StatementParser& parser, bool listen = true);

		ScriptASTPtr subParse(StatementParser& parser, const std::string& name, std::ifstream& file);
		ScriptASTPtr useImport(const std::string& name);

		const ScopedSymbolTable& link(const std::string& name) const;

		ASTNode& fromBridge(ASTNodePtr root);

		ScriptHandleAST* handle() const { return m_handle; }

		SymbolTable& symbols();
		const SymbolTable& symbols() const;

		const std::string& name() const;
		bool isBridged() const;
		bool failed() const;
		void setFailed();
		std::size_t id() const;

		void pushContext(ParsingContext context);
		void popContext();
		Token* contextOf(ParsingContextType type, std::size_t maxDepth = 0) const;

		const ASTNode& rootNode() const;
		ASTNode& rootNode();

		friend std::ostream& operator<<(std::ostream& stream, const ScriptAST&);

	private:
		std::unordered_set<std::string> m_links;
		ScriptHandleAST* m_handle = nullptr;
		ScriptCacheAST* m_cache;
		bool m_inCache = false;
	};

}
