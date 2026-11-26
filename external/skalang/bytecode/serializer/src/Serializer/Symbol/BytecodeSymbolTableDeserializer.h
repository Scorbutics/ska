#pragma once
#include <string>
#include <utility>
#include <sstream>
#include <optional>
#include "BytecodeTreeSymbolTableMapBuilder.h"
#include "BytecodeSymbolTableDeserializerHelper.h"

#include "Generator/Value/BytecodeOperand.h"
#include "Runtime/Service/ScriptTypeSerializer.h"
#include "Base/Serialization/SerializerSafeZone.h"
#include "Serializer/BytecodeChunk.h"
#include "SerializerSymbolizedType.h"

namespace ska {
	class Symbol;
	class SerializerOutput;

	namespace bytecode {
		class ScriptCache;

		class SymbolTableDeserializer {
		public:
			SymbolTableDeserializer(ScriptCache& cache);
			SymbolTableDeserializer(SymbolTableDeserializer&&) = delete;
			SymbolTableDeserializer(const SymbolTableDeserializer&) = delete;
			SymbolTableDeserializer& operator=(SymbolTableDeserializer&&) = delete;
			SymbolTableDeserializer& operator=(const SymbolTableDeserializer&) = delete;
			~SymbolTableDeserializer() = default;

			void readFull(SerializerOutput output);
			ScopedSymbolTable* read(SerializerOutput& output);
		private:
			SymbolizedType readPart(SerializerOutput& output);

			SymbolTableDeserializerHelper m_helper;
		};
	}
}
