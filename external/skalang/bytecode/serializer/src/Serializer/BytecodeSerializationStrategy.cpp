#include <string>
#include <unordered_map>
#include <fstream>
#include "BytecodeSerializationStrategy.h"
#include "Service/ScriptNameBuilder.h"

template <class T>
T& build(const std::string& scriptName) {
	const auto foundExtPos = scriptName.rfind("." SKALANG_SCRIPT_EXT);
	auto ext = std::string{ "b" };
	if (foundExtPos == std::string::npos) {
		ext = "." SKALANG_SCRIPT_EXT + ext;
	}
	auto bytecodeScriptName = scriptName + ext;
	static auto tabs = std::unordered_map<std::string, T> {};
	auto tabFound = tabs.find(bytecodeScriptName);
	if (tabFound == tabs.end()) {
		tabFound = tabs.emplace(bytecodeScriptName, T{}).first;
		tabFound->second.open(bytecodeScriptName, std::iostream::binary);
		if (tabFound->second.fail()) {
			throw std::runtime_error("opening file " + bytecodeScriptName + " failed");
		}
	}
	return tabFound->second;
}

ska::bytecode::SerializationStrategy ska::bytecode::SerializationStrategyType::PerScript() {
	return [](const std::string& scriptName) -> std::ostream& {
		return build<std::ofstream>(scriptName);
	};
}

ska::bytecode::DeserializationStrategy ska::bytecode::DeserializationStrategyType::PerScript() {
	return [](const std::string& scriptName) -> std::istream& {
		return build<std::ifstream>(scriptName);
	};
}

