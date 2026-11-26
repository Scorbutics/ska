#pragma once
#include <vector>
#include <limits>
#include <memory>
#include <unordered_map>
#include <ostream>
#include "Runtime/Value/PlainMemoryTable.h"
#include "Runtime/Value/StringShared.h"
#include "Runtime/Value/ScriptVariableRef.h"
#include "Runtime/Value/NativeFunction.h"

namespace ska {
	namespace bytecode {

		struct SymbolInfo {
			SymbolInfo() = delete;

			StringShared name;
			bool exported = false;
			std::size_t script = static_cast<std::size_t>(-1);
			std::size_t childIndex = 0;
			std::size_t binding = std::numeric_limits<std::size_t>::max();
			bool bindingPassThrough = false;

			SymbolInfo(std::size_t scopeIndex, std::string name, std::size_t scriptIndex) :
				name(std::make_shared<std::string>(scopeIndex == 1 ? std::move(name) : "")),
				script(scriptIndex)
				{}
			SymbolInfo( std::size_t scriptIndex) :
				script(scriptIndex) {}

			bool empty() const { return name == nullptr || name->empty(); }
		};

		using SymbolInfoPack = std::vector<SymbolInfo>;

		std::ostream& operator<<(std::ostream& stream, const SymbolInfo& symbol);
		std::ostream& operator<<(std::ostream& stream, const SymbolInfoPack& symbols);
	}
}
