#include "SymbolFactory.h"

// TODO simplify ?

ska::Symbol ska::SymbolFactory::make(std::size_t tableIndex, std::string name, ScopedSymbolTable& table, bool exported) {
	return { tableIndex, name, exported };
}

ska::Symbol ska::SymbolFactory::make(std::size_t tableIndex, std::string name, ScriptHandleAST& script, bool exported) {
	return { tableIndex, name, exported };
}
