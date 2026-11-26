#pragma once
#include <sstream>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_set>
#include <cstdint>
#include "NodeValue/ASTNodePtr.h"

#include "BytecodeSerializationStrategy.h"
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

		struct DeserializationScriptContext {
			DeserializationScriptContext(ScriptCache& cache, SymbolTableDeserializer& symbolsDeserializer, std::istream& input);
			DeserializationScriptContext(const DeserializationScriptContext&) = delete;
			DeserializationScriptContext(DeserializationScriptContext&&) noexcept;

			void operator>>(ScriptHeader& header);
			void operator>>(ScriptBody& body);
			void operator>>(ScriptExternalReferences& externalReferences);

			ScriptParts& parts() { return m_parts; }

		private:
			void operator>>(Instruction& value);
			void operator>>(Operand& value);

			SerializerNativeContainer extractNatives(std::istream& input);
			std::string readString();

			std::vector<ExportSymbol> readExports();
			std::unordered_set<std::string> readLinkedScripts();
			std::vector<Instruction> readInstructions();
			void readSymbolTable();

			ScriptCache& m_cache;
			SymbolTableDeserializer& m_symbolsDeserializer;
			std::stringstream m_rawInput;
			SerializerNativeContainer m_natives;
			SerializerOutput m_input;
			ScriptParts m_parts;			
		};

	}
}
