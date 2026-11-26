#pragma once
#include <sstream>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_set>
#include <cstdint>
#include "NodeValue/ASTNodePtr.h"

#include "BytecodeSerializationStrategy.h"
#include "BytecodeDeserializationScriptContext.h"
#include "BytecodeScriptParts.h"
#include "BytecodeChunk.h"
#include "Generator/Value/BytecodeExport.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Serializer/Symbol/BytecodeSymbolTableDeserializer.h"
#include "Base/Serialization/SerializerOutput.h"

namespace ska {
	namespace bytecode {
		struct ScriptHeader;
		struct ScriptBody;
		struct ScriptExternalReferences;

		struct DeserializationContext {
			DeserializationContext(ScriptCache& cache, std::string scriptStartName, DeserializationStrategy strategy);

			void declare(const std::string& scriptName, std::vector<Instruction> instructions);
			void generateExports(const std::string& scriptName);
			DeserializationScriptContext* read(const std::string& scriptName);
			bool isReadingStarted(const std::string& scriptName) const;

			const std::string& startScriptName() const { return m_scriptStartName; }
		
		private:
			ScriptCache& m_cache;
			std::string m_scriptStartName;
			DeserializationStrategy m_strategy;
			order_indexed_string_map<DeserializationScriptContext> m_scriptDeserializationContext;
			SymbolTableDeserializer m_symbolsDeserializer;
		};

	}
}
