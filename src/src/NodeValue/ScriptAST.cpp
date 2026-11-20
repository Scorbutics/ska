#include "Config/LoggerConfigLang.h"
#include <memory>
#include "ScriptAST.h"
#include "ScriptHandleAST.h"
#include "ScriptCacheAST.h"
#include "Service/StatementParser.h"
#include "Service/ASTFactory.h"
#include "Runtime/Value/NativeFunction.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptAST)

std::ostream& ska::operator<<(std::ostream& stream, const ScriptAST& ast) {
	if (ast.m_handle == nullptr || !ast.m_handle->hasRootNode()) {
		// ex : a script rebuilt from its serialized bytecode has no AST
		stream << "(empty AST)";
	} else {
		auto& rootNode = ast.m_handle->rootNode();
		stream << rootNode << std::endl;
	}
	return stream;
}

ska::ScriptAST::ScriptAST(ScriptHandleAST& handle) :
			m_cache(&handle.m_cache) {
			m_handle = &handle;
		}

ska::ScriptAST::ScriptAST(ScriptCacheAST& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex, std::size_t scriptId) :
	m_cache(&scriptCache) {
	if(m_cache->find(name) == m_cache->end()) {
		auto handle = std::unique_ptr<ScriptHandleAST>(new ScriptHandleAST{ *m_cache, std::move(input), startIndex, name });
		SLOG(LogLevel::Info) << "Adding script AST " << name << " in cache";
		m_cache->emplace(name, std::move(handle), true);
	} else {
		auto& scriptWanted = m_cache->at(name);
		if (!input.empty() && scriptWanted.m_input.emptyTokens()) {
			if (scriptWanted.m_ast != nullptr) {
				throw std::runtime_error("invalid ast script instantiation : tokens were provided but ast is already built");
			}
			scriptWanted.m_input = TokenReader{ std::move(input) };

			//m_inCache = true;
		} else {
			SLOG(LogLevel::Info) << "Script AST " << name << " is already in cache";
			m_inCache = true;
		}
	}
	m_handle = &m_cache->at(name);
}

const ska::ScopedSymbolTable& ska::ScriptAST::link(const std::string& name) const {
	// First, try to ensure we have the authorization by checking if the import has been done in the current script
	const auto hasBeenImported = m_links.find(name) != m_links.end();
	if (!hasBeenImported) {
		throw std::runtime_error("cannot access script \"" + name + "\" from \"" + m_handle->m_name + "\" because it has not been imported");
	}
	return m_cache->at(name).m_symbols.root();
}

ska::ScriptASTPtr ska::ScriptAST::useImport(const std::string& name) {
	auto result = existsInCache(name) ? std::make_unique<ScriptAST>(*m_cache, name, std::vector<Token>{}) : nullptr;
	m_links.emplace(name);
	return result;
}

ska::ASTNode& ska::ScriptAST::fromBridge(ASTNodePtr astRoot) {
	assert(m_handle->m_ast == nullptr && "Script built from a bridge must be empty");


	m_handle->m_ast = std::move(astRoot);
	m_handle->m_bridged = true;
	return *m_handle->m_ast;
}

ska::ScriptASTPtr ska::ScriptAST::subParse(StatementParser& parser, const std::string& name, std::ifstream& file) {
	return parser.subParse(m_handle->m_cache, name, file);
}

void ska::ScriptAST::parse(StatementParser& parser, bool listen) {
	if(m_inCache) {
		//No more parsing if script is already in-cache.
		return;
	}

	if(!listen) {
		m_handle->m_ast = parser.parse(*this);
		return;
	}

	auto lock = m_handle->m_symbols.listenParser(parser);
	auto result = parser.parse(*this);
	lock.release();
	m_handle->m_ast = std::move(result);
	return;
}

ska::ASTNodePtr ska::ScriptAST::statement(StatementParser& parser) {
	return parser.statement(*this);
}

ska::ASTNodePtr ska::ScriptAST::optstatement(StatementParser& parser, const Token& mustNotBe) {
	return parser.optstatement(*this, mustNotBe);
}

ska::ASTNodePtr ska::ScriptAST::expr(StatementParser& parser) {
	return parser.expr(*this);
}

ska::ASTNodePtr ska::ScriptAST::optexpr(StatementParser& parser, const Token& mustNotBe) {
	return parser.optexpr(*this, mustNotBe);
}

void ska::ScriptAST::pushContext(ParsingContext context) {
	m_handle->m_parsingContexts.push_back(std::move(context));
}

void ska::ScriptAST::popContext() {
	if (!m_handle->m_parsingContexts.empty()) {
		m_handle->m_parsingContexts.pop_back();
	}
}

ska::Token* ska::ScriptAST::contextOf(ParsingContextType type, std::size_t maxCount) const {
	std::size_t countIndex = 0;
	for (auto it = m_handle->m_parsingContexts.rbegin(); it != m_handle->m_parsingContexts.rend() && (maxCount == 0 || countIndex < maxCount); it++) {
		if (it->type == type) {
			countIndex++;
			if (countIndex >= maxCount) {
				return &it->owner;
			}
		}
	}
	return nullptr;
}

ska::SymbolTable &ska::ScriptAST::symbols() {
	return m_handle->m_symbols;
}

const ska::SymbolTable& ska::ScriptAST::symbols() const { return m_handle->m_symbols; }

const std::string& ska::ScriptAST::name() const { return m_handle->name(); }

const ska::ASTNode& ska::ScriptAST::rootNode() const {
	assert(m_handle->m_ast != nullptr);
	return *m_handle->m_ast;
}

ska::ASTNode& ska::ScriptAST::rootNode() {
	assert(m_handle->m_ast != nullptr);
	return *m_handle->m_ast;
}

bool ska::ScriptAST::isBridged() const { return m_handle != nullptr && m_handle->m_bridged; }

std::size_t ska::ScriptAST::id() const { return m_handle->id(); }

bool ska::ScriptAST::existsInCache(const std::string& name) const {
	return m_cache->find(name) != m_cache->end();
}

void ska::ScriptAST::rewind() {
	return m_handle->m_input.rewind();
}

ska::TokenReader& ska::ScriptAST::reader() { return m_handle->m_input; }
