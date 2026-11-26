#include "Serializer/Config/LoggerSerializer.h"
#include "BytecodeDeserializationScriptContext.h"
#include "BytecodeScriptExternalReferences.h"
#include "BytecodeScriptHeader.h"
#include "BytecodeScriptBody.h"
#include "Base/Serialization/SerializerOutput.h"
#include "BytecodeOperandSerializer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::DeserializationScriptContext);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::DeserializationScriptContext)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::DeserializationScriptContext)

ska::bytecode::DeserializationScriptContext::DeserializationScriptContext(ScriptCache& cache, SymbolTableDeserializer& symbolsDeserializer, std::istream& input) :
	m_cache(cache),
	m_symbolsDeserializer(symbolsDeserializer),
	m_natives(extractNatives(input)),
	m_input(SerializerOutput{ { m_rawInput, m_natives} }),
	m_parts(ScriptParts{ m_natives }) {
}

ska::bytecode::DeserializationScriptContext::DeserializationScriptContext(DeserializationScriptContext&& context) noexcept :
	m_cache(context.m_cache),
	m_symbolsDeserializer(context.m_symbolsDeserializer),
	m_rawInput(std::move(context.m_rawInput)),
	m_natives(std::move(context.m_natives)),
	m_input(SerializerOutput{ {m_rawInput, m_natives } }),
	m_parts(ScriptParts{ m_natives }) {
}

void ska::bytecode::DeserializationScriptContext::operator>>(ScriptHeader& header) {
	auto safeZoneHeader = m_input.acquireMemory<3 * sizeof(uint32_t) + sizeof(Chunk)>("Script header");
	header.serializerVersion = safeZoneHeader.read<uint32_t>();
	header.scriptNameRef = safeZoneHeader.read<Chunk>();
	header.scriptId = safeZoneHeader.read<uint32_t>();
	header.scriptBridged = safeZoneHeader.read<uint32_t>();
}

void ska::bytecode::DeserializationScriptContext::operator>>(ScriptBody& body) {
	body.instructions = readInstructions();
	readSymbolTable();
}

void ska::bytecode::DeserializationScriptContext::operator>>(ScriptExternalReferences& externalReferences) {
	externalReferences.scripts = readLinkedScripts();
}

std::vector<ska::bytecode::Instruction> ska::bytecode::DeserializationScriptContext::readInstructions() {
	auto instructionsSize = m_input.acquireMemory<sizeof(uint32_t)>("instructions size").read<uint32_t>();
	
	if (instructionsSize == 0) {
		return {};
	}

	auto instructions = std::vector<Instruction>(instructionsSize);
	for (std::size_t i = 0; i < instructionsSize; i++){
		(*this) >> instructions[i];
		LOG_INFO << "Deserializing " << instructions[i];
	}
	return instructions;
}

std::unordered_set<std::string> ska::bytecode::DeserializationScriptContext::readLinkedScripts() {
	auto linkedScriptsRef = std::unordered_set<std::string>{};
	auto linkedScriptsRefSize = m_input.acquireMemory<sizeof(uint32_t)>("linkedScriptsRef size").read<uint32_t>();

	LOG_INFO << "Linked scripts section : " << linkedScriptsRefSize;

	for (std::size_t i = 0; i < linkedScriptsRefSize; i++) {
		auto scriptRef = m_input.acquireMemory<sizeof(Chunk)>("linkedScriptsRef id").read<Chunk>();
		if (scriptRef >= m_natives.size()) { 
			throw std::runtime_error("bad bytecode : script reference \"" + std::to_string(scriptRef) + "\" has no native correspondance");
		}
		LOG_INFO << "Getting script reference " << m_natives[scriptRef];
		linkedScriptsRef.insert(m_natives[scriptRef]);
	}
	return linkedScriptsRef;
}

void ska::bytecode::DeserializationScriptContext::readSymbolTable() {
	m_symbolsDeserializer.readFull(m_input);
}

void ska::bytecode::DeserializationScriptContext::operator>>(Instruction& value) {
	auto operands = std::vector<OperandUse>{};
	auto safeZone = m_input.acquireMemory<sizeof(uint16_t) + sizeof(uint8_t)>("Instruction");

	const auto command = safeZone.read<uint16_t>();
	const auto numberOfValidOperands = safeZone.read<uint8_t>();
	for(uint8_t i = 0; i < numberOfValidOperands; i++) {
		auto tmp = Operand {};
		*this >> tmp;
		operands.push_back(std::move(tmp));
	}
	value = Instruction{ static_cast<Command>(command), std::move(operands) };
}

void ska::bytecode::DeserializationScriptContext::operator>>(Operand& value) {
	value = OperandSerializer::read(m_cache, m_input.acquireMemory<2 * sizeof(Chunk) + sizeof(uint8_t)>("Operand"));
}

ska::SerializerNativeContainer ska::bytecode::DeserializationScriptContext::extractNatives(std::istream& input) {
	m_rawInput << input.rdbuf();

	LOG_INFO << "Natives section ";
	SerializerNativeContainer natives;
	std::size_t nativesSize = 0;
	m_rawInput.read(reinterpret_cast<char*>(&nativesSize), sizeof(uint32_t));
	for (std::size_t i = 0; i < nativesSize; i++) {
		auto native = readString();
		LOG_INFO << i << "\t: " << native;
		auto cpNative = native;
		natives.emplace(std::move(native), std::move(cpNative));
	}
	return natives;
}

std::string ska::bytecode::DeserializationScriptContext::readString() {
	auto value = std::string {};
	Chunk size = 0;
	m_rawInput.read(reinterpret_cast<char*>(&size), sizeof(Chunk));
	if (size > 0 && size <= std::numeric_limits<std::size_t>::max()) {
		value.resize(size);
		m_rawInput.read(&value[0], size * sizeof(char));
		return value;
	}
	return "";
}
