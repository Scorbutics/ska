#pragma once
#include <string>
#include "NodeValue/Symbol.h"

namespace ska {
	class ScopedSymbolTable;
	struct ScriptHandleAST;

	class SymbolFactory {
	public:
		~SymbolFactory() = default;
	protected:
		SymbolFactory() = default;

		Symbol make(std::size_t tableIndex, std::string name, ScopedSymbolTable& table, bool exported);
		Symbol make(std::size_t tableIndex, std::string name, ScriptHandleAST& script, bool exported);
	};
}
