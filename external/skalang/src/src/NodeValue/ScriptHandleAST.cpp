#include "Config/LoggerConfigLang.h"
#include "ScriptCacheAST.h"
#include "ScriptHandleAST.h"

ska::ScriptHandleAST::ScriptHandleAST(ScriptCacheAST& cache, std::vector<Token> input, std::size_t startIndex, std::string name) :
	m_cache(cache),
	m_input(std::move(input), startIndex),
	m_name(std::move(name)),
	m_id(m_cache.id(m_name)),
	m_symbols(m_name) {
}
