#include "Serializer/Config/LoggerSerializer.h"
#include "BytecodeSymbolTableDeserializer.h"
#include "Serializer/BytecodeCommonSerializer.h"
#include "Serializer/BytecodeOperandSerializer.h"
#include "Generator/Value/BytecodeScriptCache.h"

#include "Service/ScopedSymbolTable.h"
#include "SerializerSymbol.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::SymbolTableDeserializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SymbolTableDeserializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SymbolTableDeserializer)

ska::bytecode::SymbolTableDeserializer::SymbolTableDeserializer(ScriptCache& cache) :
	m_helper(cache) {
}

void ska::bytecode::SymbolTableDeserializer::readFull(SerializerOutput output) {
	const std::size_t totalSymbolsInMap = output.acquireMemory<sizeof(uint32_t)>("Symbol table : total number of symbols").read<uint32_t>();

	LOG_INFO << "Symbols (in total) : " << totalSymbolsInMap;

	for (std::size_t i = 0; i < totalSymbolsInMap; i++) {
		readPart(output);
	}

	m_helper.applyDeferredImplements();
	output.validateOrThrow();
}

ska::ScopedSymbolTable* ska::bytecode::SymbolTableDeserializer::read(SerializerOutput& output) {
	auto symbolSerializer = SerializerType<ska::ScopedSymbolTable*, SymbolTableDeserializerHelper&>{output};
	ScopedSymbolTable* symbolTable;
	symbolSerializer.read(symbolTable, m_helper);
	return symbolTable;
}

ska::SymbolizedType ska::bytecode::SymbolTableDeserializer::readPart(SerializerOutput& output) {
	auto symbolizedTypeSerializer = SerializerType<SymbolizedType, SymbolTableDeserializerHelper&>{ output };
	auto symbolizedType = SymbolizedType{};
	symbolizedTypeSerializer.read(symbolizedType, m_helper);

	LOG_INFO << "\t\tType " << symbolizedType.type;
	LOG_INFO << "%13c\t\twith Raw type : " << ExpressionTypeSTR[static_cast<std::size_t>(symbolizedType.type.type())];
	LOG_INFO << "%13c\t\twith " << symbolizedType.compoundTypes << " children";

	for (std::size_t index = 0; index < symbolizedType.compoundTypes; index++) {
		auto childSymbolizedType = readPart(output);
		symbolizedType.type.add(std::move(childSymbolizedType.type));
	}

	if (symbolizedType.symbolTable != nullptr) {
		symbolizedType.symbolTable->changeTypeIfRequired(symbolizedType.type);
	}

	return symbolizedType;
}

