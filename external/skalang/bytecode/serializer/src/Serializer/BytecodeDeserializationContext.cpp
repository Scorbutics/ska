#include "Serializer/Config/LoggerSerializer.h"
#include "BytecodeDeserializationContext.h"
#include "BytecodeScriptExternalReferences.h"
#include "BytecodeScriptHeader.h"
#include "BytecodeScriptBody.h"
#include "Base/Serialization/SerializerOutput.h"
#include "BytecodeOperandSerializer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::DeserializationContext);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::DeserializationContext)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::DeserializationContext)

ska::bytecode::DeserializationContext::DeserializationContext(ScriptCache& cache, std::string scriptStartName, DeserializationStrategy strategy) :
	m_strategy(std::move(strategy)),
	m_scriptStartName(std::move(scriptStartName)),
	m_cache(cache),
	m_symbolsDeserializer(cache) {
}

bool ska::bytecode::DeserializationContext::isReadingStarted(const std::string& scriptName) const {
	return m_scriptDeserializationContext.atOrNull(scriptName) != nullptr;
}

ska::bytecode::DeserializationScriptContext* ska::bytecode::DeserializationContext::read(const std::string& scriptName) {
	try {
		auto& input = m_strategy(scriptName);
		if (input.eof()) {
			return nullptr;
		}

		auto* scriptDeserializationContext = m_scriptDeserializationContext.atOrNull(scriptName);
		if (scriptDeserializationContext == nullptr) {
			auto* ast = m_cache.astCache.atOrNull(scriptName);
			if (ast == nullptr) {
				auto fakeAST = ska::ScriptAST{ m_cache.astCache, scriptName, {} };
				m_cache.emplace(scriptName, ScriptGeneration{ ScriptGenerationHelper{ m_cache, fakeAST } });
			} else {
				m_cache.emplace(scriptName, ScriptGeneration{ ScriptGenerationHelper{ m_cache, *ast } });
			}
			
			m_scriptDeserializationContext.emplace(scriptName, DeserializationScriptContext{m_cache, m_symbolsDeserializer, input });
		}

		return &m_scriptDeserializationContext.at(scriptName);
	} catch (std::runtime_error& e) { 
		// unexisting script
		std::cerr << e.what() << std::endl;
		return nullptr;
	}
}

void ska::bytecode::DeserializationContext::declare(const std::string& scriptName, std::vector<Instruction> instructions) {
	auto output = InstructionOutput {};
	for (auto& ins : instructions) {
		output.push(std::move(ins));
	}
	auto helper = std::move(m_cache.at(scriptName).helper());
	m_cache.at(scriptName) = std::move(ScriptGeneration{ std::move(helper), std::move(output) });
	LOG_INFO << "Script \"" << scriptName << "\" has index " << m_cache.id(scriptName);
}


void ska::bytecode::DeserializationContext::generateExports(const std::string& scriptName) {
	m_cache.getExportedSymbols(m_cache.id(scriptName));
	LOG_INFO << "Script \"" << scriptName << "\" has exports generated sucessfully";
}
