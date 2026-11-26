#include "Serializer/Config/LoggerSerializer.h"
#include <unordered_set>
#include "BytecodeSerializer.h"
#include "BytecodeSerializationContext.h"
#include "BytecodeDeserializationContext.h"

#include "BytecodeScriptHeader.h"
#include "BytecodeScriptBody.h"
#include "BytecodeScriptExternalReferences.h"


SKA_LOGC_CONFIG(ska::LogLevel::Error, ska::bytecode::Serializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::Serializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::Serializer)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::Serializer)

bool ska::bytecode::Serializer::serialize(SerializationContext& context) const {
	try {
		auto partIndexes = std::deque<std::size_t>{};
		do {
			partIndexes.push_back(context.writeHeader(m_version));

			if (!context.currentScriptBridged()) {
				auto [instructionIndex, linkedScripts] = context.writeInstructions();
				auto symbolTableIndex = context.writeSymbolTable();
				partIndexes.push_back(context.writeExternalReferences(std::move(linkedScripts)));
				partIndexes.push_back(instructionIndex);
				partIndexes.push_back(symbolTableIndex);
			}
			
		} while (context.next(std::move(partIndexes)));
		return true;
	} catch (std::runtime_error& e) {
		LOG_ERROR << e.what();
		return false;
	}
}

std::vector<std::string> ska::bytecode::Serializer::deserialize(DeserializationContext& context, const std::string& scriptName, const std::unordered_set<std::string>& blacklist) const {
	if (context.isReadingStarted(scriptName)) {
		return {};
	}
	
	auto scriptSerializationContext = context.read(scriptName);

	auto failedScripts = std::vector<std::string>{};

	if (scriptSerializationContext != nullptr) {

		auto& script = scriptSerializationContext->parts();

		*scriptSerializationContext >> script.header;

		LOG_INFO << "[Serializer script version " << script.header.serializerVersion << "], script " << script.header.scriptName() << " refered as id " << script.header.scriptId << " when it was compiled";

		if (script.header.serializerVersion != m_version) {
			LOG_INFO << "Script " << scriptName << " was serialized with the format version " << script.header.serializerVersion << " instead of " << m_version << ". Added to queue.";
			failedScripts.push_back(scriptName);
			return failedScripts;
		}

		if (!script.header.scriptBridged) {
			*scriptSerializationContext >> script.references;

			for (const auto& referencedScript : script.references.scripts) {
				auto referencedFailedScripts = deserialize(context, referencedScript, blacklist);
				failedScripts.insert(failedScripts.end(), std::make_move_iterator(referencedFailedScripts.begin()),
					std::make_move_iterator(referencedFailedScripts.end()));
			}

			script.references.scripts.clear();


		if (blacklist.find(script.header.scriptName()) == blacklist.end()) {
				*scriptSerializationContext >> script.body;

				LOG_INFO << "Declaring script " << script.header.scriptName() << (script.header.scriptBridged ? " (bridged)" : "");
			context.declare(script.header.scriptName(), std::move(script.body.instructions));

				context.generateExports(script.header.scriptName());

			}
			else {
			LOG_INFO << "Script " << script.header.scriptName() << " is blacklisted. Not declared";
		}
		}
	} else {
		failedScripts.push_back(scriptName);
		LOG_INFO << "Script " << scriptName << " cannot be read. Added to queue.";
	}
		
	return failedScripts;
	
}

std::vector<std::string> ska::bytecode::Serializer::deserialize(DeserializationContext& context, const std::unordered_set<std::string>& blacklist) const {
	std::string scriptName = context.startScriptName();
	return deserialize(context, scriptName, blacklist);
}

bool ska::bytecode::Serializer::serialize(ScriptCache& cache, SerializationStrategy output) const {
	auto context = SerializationContext { cache, output };
	return serialize(context);
}

std::vector<std::string> ska::bytecode::Serializer::deserialize(ScriptCache& cache, const std::string& startScriptName, DeserializationStrategy input, const std::unordered_set<std::string>& blacklist) const {
	auto context = DeserializationContext{ cache, startScriptName, input };
	return deserialize(context, blacklist);
}
