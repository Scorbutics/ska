#include "Config/LoggerConfigLang.h"
#include "Service/ScopedSymbolTable.h"

#include "SerializerSymbol.h"
#include "BytecodeSymbolTableDeserializer.h"
#include "BytecodeSymbolTableSerializer.h"

namespace ska {
	struct SerializerTypeTraitsSymbolLog;
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SerializerTypeTraitsSymbolLog);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolLog)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::SerializerTypeTraitsSymbolLog)

void ska::SerializerTypeTraits<ska::ScopedSymbolTable*>::Read(SerializerSafeZone<BytesRequired>& zone, ScopedSymbolTable*& symbol, bytecode::SymbolTableDeserializerHelper& helper) {
	symbol = &ReadSymbolRefBody(zone.acquireMemory<BytesSymbolRefRequired>("Symbol ref"), helper);

	const auto hasMaster = zone.read<uint8_t>() != 0;
	if (!hasMaster) {
		LOG_INFO << "%13cNo parent symbol";
		auto safeNullZone = zone.acquireMemory<BytesSymbolRefRequired>("No parent");
		safeNullZone.readNull<BytesSymbolRefRequired>();
	} else {
		LOG_INFO << "%13c\twith master : ";
		auto& master = ReadSymbolRefBody(zone.acquireMemory<BytesSymbolRefRequired>("Master symbol ref"), helper);
		helper.deferImplement(master, *symbol);
	}
}

ska::ScopedSymbolTable& ska::SerializerTypeTraits<ska::ScopedSymbolTable*>::ReadSymbolRefBody(SerializerSafeZone<BytesSymbolRefRequired> zone, bytecode::SymbolTableDeserializerHelper& helper) {
	auto absoluteScriptKey = zone.read<std::string>();
	auto symbolName = zone.read<std::string>();
	LOG_INFO << "%13c" << symbolName << " with key : " << absoluteScriptKey;
	auto operand = helper.readOperand(zone.acquireMemory<sizeof(uint8_t) + sizeof(bytecode::Chunk) * 2>("symbol operand"));
	const auto exported = zone.read<uint8_t>() != 0;
	return helper.buildSymbol(zone, absoluteScriptKey, symbolName, operand, exported);
}

void ska::SerializerTypeTraits<ska::ScopedSymbolTable*>::Write(SerializerSafeZone<BytesRequired>& zone, const ScopedSymbolTable& symbolTable, bytecode::SymbolTableSerializerHelper& helper) {
	WriteSymbolRefBody(zone.acquireMemory<BytesSymbolRefRequired>("Symbol ref"), symbolTable, helper);

	const auto hasMaster = symbolTable.classTable() != nullptr;
	zone.write(static_cast<uint8_t>(hasMaster));
	if (!hasMaster) {
		zone.writeNull<BytesSymbolRefRequired>();
	} else {
		LOG_INFO << "%13c\twith master : " << symbolTable.classTable()->name();
		WriteSymbolRefBody(zone.acquireMemory<BytesSymbolRefRequired>("parent"), *symbolTable.classTable(), helper);
	}
}

void ska::SerializerTypeTraits<ska::ScopedSymbolTable*>::WriteSymbolRefBody(SerializerSafeZone<BytesSymbolRefRequired> zone, const ScopedSymbolTable& symbolTable, bytecode::SymbolTableSerializerHelper& helper) {
	auto scriptId = helper.scriptOfSymbol(symbolTable);
	auto operand = helper.operandOfSymbol(symbolTable);
	auto relativeScriptKey = helper.getRelativeScriptKey(scriptId, symbolTable);
	auto absoluteScriptKey = std::to_string(zone.ref(helper.getScriptName(scriptId))) + (relativeScriptKey.empty() ? "" : ("." + relativeScriptKey));

	LOG_INFO << "%13cSymbol Name refered : " << symbolTable.name() << " with key : " << absoluteScriptKey;

	zone.write(std::move(absoluteScriptKey));
	zone.write(symbolTable.name());
	helper.writeOperand(zone.acquireMemory<sizeof(uint8_t) + sizeof(bytecode::Chunk) * 2>("symbol operand"), operand);
	// "exported" symbols are the ones indexed as fields of their parent (declarations, not parameters) : see ScopedSymbolTable::id
	zone.write(static_cast<uint8_t>(symbolTable.symbol() != nullptr && symbolTable.symbol()->exported()));
}

