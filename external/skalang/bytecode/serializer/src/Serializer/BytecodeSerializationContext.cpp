#include "Config/LoggerConfigLang.h"
#include "Serializer/Config/LoggerSerializer.h"
#include "BytecodeSerializationContext.h"
#include "BytecodeCommonSerializer.h"
#include "BytecodeOperandSerializer.h"
#include "Base/Serialization/SerializerOutput.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::SerializationContext);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SerializationContext)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SerializationContext)

ska::bytecode::SerializationContext::SerializationContext(ScriptCache& cache, SerializationStrategy strategy) :
	m_strategy(std::move(strategy)),
	m_cache(cache),
	m_output(&m_strategy(cache.at(m_id).name())),
	m_symbolsSerializer(cache) {
}

bool ska::bytecode::SerializationContext::next(std::deque<std::size_t> partIndexes) {
	partIndexes.push_front(pushNatives());
	commit(std::move(partIndexes));
	m_id++;
	if (m_cache.exist(m_id)) {
		m_output = &m_strategy(m_cache[m_id].name());
		return true;
	}
	return false;
}

std::size_t ska::bytecode::SerializationContext::writeHeader(std::size_t serializerVersion) {
	push();
	LOG_DEBUG << "Serializing script " << currentScriptName() << " with compiled id " << currentScriptId();
	auto output = SerializerOutput{ {buffer(), m_natives} };
	output.acquireMemory<sizeof(uint32_t)>("script serializer version").write(static_cast<uint32_t>(serializerVersion));
	output.acquireMemory<sizeof(Chunk)>("script name").write(currentScriptName());
	output.acquireMemory<sizeof(uint32_t)>("script compile id").write(static_cast<uint32_t>(currentScriptId()));
	output.acquireMemory<sizeof(uint32_t)>("is script bridged").write(static_cast<std::uint32_t>(currentScriptBridged()));
	output.validateOrThrow();
	return m_buffer.size() - 1;
}

std::pair<std::size_t, std::vector<std::string>> ska::bytecode::SerializationContext::writeInstructions() {
	push();
	auto linkedScripts = std::vector<std::string>{};
	auto output = SerializerOutput{ {buffer(), m_natives} };
	output.acquireMemory<sizeof(uint32_t)>("instructions size").write(static_cast<uint32_t>(instructionsSize()));
	for (const Instruction& instruction : (*this)) {
		LOG_DEBUG << "Serializing " << instruction;
		(*this) << instruction;
		if (instruction.command() == Command::SCRIPT) {
			linkedScripts.push_back(scriptName(instruction.left().as<ScriptVariableRef>().variable));
		}
	}
	
	return std::make_pair(m_buffer.size() - 1, std::move(linkedScripts));
}

std::size_t ska::bytecode::SerializationContext::writeExternalReferences(std::vector<std::string> linkedScripts) {
	push();
	auto output = SerializerOutput{ SerializerOutputData{ buffer(), m_natives } };
	output.acquireMemory<sizeof(uint32_t)>("linkedScripts (size)").write(static_cast<uint32_t>(linkedScripts.size()));
	for (const auto& linkedScript : linkedScripts) {
		LOG_INFO << linkedScript;
		output.acquireMemory<sizeof(Chunk)>("linkedScript name").write(linkedScript);
	}
	output.validateOrThrow();
	return m_buffer.size() - 1;
}

std::size_t ska::bytecode::SerializationContext::writeSymbolTable() {
	push();
	m_symbolsSerializer.writeFull(SerializerOutput{ {buffer(), m_natives} }, m_id);
	return m_buffer.size() - 1;
}

void ska::bytecode::SerializationContext::operator<<(const Instruction& value) {
	uint16_t cmd = static_cast<uint16_t>(value.command());
	uint8_t numberOfValidOperands = !value.dest().empty() + !value.left().empty() + !value.right().empty();
	buffer().write(reinterpret_cast<const char*>(&cmd), sizeof(uint16_t));
	buffer().write(reinterpret_cast<const char*>(&numberOfValidOperands), sizeof(uint8_t));

	if (numberOfValidOperands-- > 0) *this << value.dest(); else return;
	if (numberOfValidOperands-- > 0) *this << value.left(); else return;
	if (numberOfValidOperands-- > 0) *this << value.right();
}

void ska::bytecode::SerializationContext::operator<<(const Operand& value) {
	auto output = SerializerOutput{ {buffer(), m_natives} };
	OperandSerializer::write(m_cache, output.acquireMemory<2 * sizeof(Chunk) + sizeof(uint8_t)>("Operand"), value);
	output.validateOrThrow();
}

void ska::bytecode::SerializationContext::commit(std::deque<std::size_t> partIndexes) {
	auto target = std::vector<std::stringstream>();	
	assert(partIndexes.size() == m_buffer.size());
	for (auto index : partIndexes) {
		if (index < m_buffer.size()) {
			target.push_back(std::move(m_buffer[index]));
			m_buffer[index] = std::stringstream{};
		}
	}

	for (auto& buf : target) {
		(*m_output) << buf.rdbuf();
	}
	m_buffer.clear();
}

void ska::bytecode::SerializationContext::push() {
	m_buffer.emplace_back();
}

std::size_t ska::bytecode::SerializationContext::pushNatives() {
	push();
	if (m_natives.size() != 0) {
		auto nativeVector = std::vector<std::string>(m_natives.size());
		LOG_DEBUG << "Total of " << nativeVector.size() << " natives";
		for (auto& native : m_natives) {
			auto index = m_natives.id(*native);
			LOG_DEBUG << "Building native vector [" << index << "] = " << native;
			nativeVector[index] = *native;
		}

		const auto totalSize = nativeVector.size();
		LOG_INFO << "Natives : " << totalSize;
		
		buffer().write(reinterpret_cast<const char*>(&totalSize), sizeof(uint32_t));
		
		for (auto& native : nativeVector) {
			LOG_INFO << native;
			const auto size = native.size();
			buffer().write(reinterpret_cast<const char*>(&size), sizeof(Chunk));
			if(size > 0) {
				buffer().write(native.c_str(), sizeof(char) * size);
			}
		}
	}
	m_natives.clear();
	return m_buffer.size() - 1;
}
