#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <utility>
#include "Serializer/Symbol/BytecodeTreeMapSymbolTableBuilder.h"
#include "Generator/Value/BytecodeOperand.h"
#include "Base/Serialization/SerializerSafeZone.h"

namespace ska {

	namespace bytecode {
		class ScriptCache;

		class SymbolTableDeserializerHelper {
		public:
			SymbolTableDeserializerHelper(ScriptCache& cache);

			SymbolTableDeserializerHelper(SymbolTableDeserializerHelper&&) = delete;
			SymbolTableDeserializerHelper(const SymbolTableDeserializerHelper&) = delete;
			SymbolTableDeserializerHelper& operator=(SymbolTableDeserializerHelper&&) = delete;
			SymbolTableDeserializerHelper& operator=(const SymbolTableDeserializerHelper&) = delete;
			~SymbolTableDeserializerHelper() = default;


			TreeMapSymbolTableBuilder& getSymbolTableBuilder(const std::string& scriptName);
			ScopedSymbolTable& buildSymbol(detail::SerializerSafeZone& zone, const std::string& absoluteScriptKey, std::string symbolName, const Operand& operand, bool exported);
			Operand readOperand(SerializerSafeZone<17> safeZone);

			// "classTable.implement(instanceTable)" locks the class table : it must be done once all types are read,
			// otherwise reading the class table type afterwards would fail.
			void deferImplement(ScopedSymbolTable& classTable, ScopedSymbolTable& instanceTable);
			void applyDeferredImplements();

		public:
			ScriptCache* m_cache = nullptr;
			std::unordered_map<std::string, std::unique_ptr<TreeMapSymbolTableBuilder>> m_symbolTableBuilder;
			std::vector<std::pair<ScopedSymbolTable*, ScopedSymbolTable*>> m_deferredImplements;
		};
	}
}
