#pragma once

#include "Base/Serialization/SerializerType.h"
#include "BytecodeSymbolTableSerializerHelper.h"
#include "NodeValue/Type.h"
#include "Serializer/BytecodeChunk.h"
#include "SerializerSymbol.h"
namespace ska {
	class ScopedSymbolTable;
	class Symbol;

	namespace bytecode {
		class SymbolTableSerializerHelper;
		class SymbolTableDeserializerHelper;
	}

	struct CSymbolizedType {
		const Type& type;
		const ScopedSymbolTable* symbolTable = nullptr;
	};

	struct SymbolizedType {
		Type type {};
		ScopedSymbolTable* symbolTable = nullptr;
		std::size_t compoundTypes = 0;
	};

	template <>
	struct SerializerTypeTraits<CSymbolizedType> {
		static constexpr std::size_t BytesRequired = 2 * sizeof(uint8_t) + sizeof(uint32_t) + SerializerTypeTraits<ScopedSymbolTable*>::BytesRequired;
		static constexpr const char* Name = "CSymbolizedType";

		static void Read(SerializerSafeZone<BytesRequired>& zone, CSymbolizedType& symbolizedType, bytecode::SymbolTableDeserializerHelper&) { assert(false); }
		static void Write(SerializerSafeZone<BytesRequired>& zone, const CSymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper&);
	};

	template <>
	struct SerializerTypeTraits<SymbolizedType> {
		static constexpr std::size_t BytesRequired = SerializerTypeTraits<CSymbolizedType>::BytesRequired;
		static constexpr const char* Name = "SymbolizedType";

		static void Read(SerializerSafeZone<BytesRequired>& zone, SymbolizedType& symbolizedType, bytecode::SymbolTableDeserializerHelper&);
		static void Write(SerializerSafeZone<BytesRequired>& zone, const SymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper&){ assert(false); }
	};
}
