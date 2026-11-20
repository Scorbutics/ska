#pragma once

#include "Base/Serialization/SerializerType.h"
#include "Serializer/BytecodeChunk.h"

namespace ska {
	class ScopedSymbolTable;
	namespace bytecode {
		class SymbolTableDeserializerHelper;
		class SymbolTableSerializerHelper;
	}

	template <std::size_t Bytes>
	struct BytesValue {
		static constexpr auto value = Bytes;
	};

	template <>
	struct SerializerTypeTraits<ScopedSymbolTable*> {
		// key + name + operand (1 byte + 2 chunks) + exported flag (1 byte)
		static constexpr std::size_t BytesSymbolRefRequired = 4 * sizeof(bytecode::Chunk) + 2 * sizeof(uint8_t);
		static constexpr std::size_t BytesRequired = 2 * BytesValue<BytesSymbolRefRequired>::value + sizeof(uint8_t);

		static constexpr const char* Name = "Symbol";

		static void Read(SerializerSafeZone<BytesRequired>& zone, ScopedSymbolTable*& symbol, bytecode::SymbolTableDeserializerHelper& helper);
		static void Write(SerializerSafeZone<BytesRequired>& zone, const ScopedSymbolTable& symbol, bytecode::SymbolTableSerializerHelper& helper);

	private:
		static ScopedSymbolTable& ReadSymbolRefBody(SerializerSafeZone<BytesSymbolRefRequired> zone, bytecode::SymbolTableDeserializerHelper& helper);
		static void WriteSymbolRefBody(SerializerSafeZone<BytesSymbolRefRequired> zone, const ScopedSymbolTable& symbol, bytecode::SymbolTableSerializerHelper& helper);
	};
}
