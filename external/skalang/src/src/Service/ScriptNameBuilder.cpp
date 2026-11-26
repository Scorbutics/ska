#include "Config/LoggerConfigLang.h"
#include "ScriptNameBuilder.h"

namespace ska {
	struct ScriptNameDeduceTag;
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptNameDeduceTag)

std::string& ska::ScriptNameStandardLibraryPath() {
	static std::string prefix = "" SKALANG_SRC_DIR "/../std/src/std/scripts";
	return prefix;
}

std::unordered_map<std::string, std::string>& ska::ScriptNameBindingAliases() {
	static auto aliases = std::unordered_map<std::string, std::string>{};
	return aliases;
}

std::string ska::ScriptNameDeduce(const std::string& scriptCaller, const std::string& scriptCalled, const std::string& stdPath) {
	if(ska::FileUtils::isAbsolutePath(scriptCalled)) {
		const auto lastDotPos = scriptCalled.find_last_of('.');
		const auto ext = (lastDotPos == std::string::npos || scriptCalled.substr(lastDotPos) != "." SKALANG_SCRIPT_EXT) ? "." SKALANG_SCRIPT_EXT : "";
		const auto fileScriptCalled = scriptCalled + ext;
		return ska::FileUtils::getCanonicalPath(fileScriptCalled);
	}

	auto strategy = ska::ScriptNameStrategy::RELATIVE_TO_PARENT;
	auto scriptCalledName = std::string{};
	const auto namespaceSeparatorPosition = scriptCalled.find_first_of(':');
	if(namespaceSeparatorPosition != std::string::npos) {
		auto namespace_ = scriptCalled.substr(0, namespaceSeparatorPosition);
		SLOG_STATIC(LogLevel::Debug, ska::ScriptNameDeduceTag) << "Reading script namespace : " << namespace_ << " for script " << scriptCalled;
		if(namespace_ == "std") {
			strategy = ska::ScriptNameStrategy::STD_DIRECTORY;
		} else if(namespace_ == "wd") {
			strategy = ska::ScriptNameStrategy::WORKING_DIRECTORY;
		} else if (namespace_ == "bind") {
			const auto& aliases = ScriptNameBindingAliases();
			const auto alias = aliases.find(scriptCalled.substr(namespaceSeparatorPosition + 1));
			if (alias != aliases.end()) {
				return ska::FileUtils::getCanonicalPath(alias->second);
			}
			strategy = ska::ScriptNameStrategy::RELATIVE_TO_RUNNER;
		} else {
			throw std::runtime_error("undefined namespace \"" + namespace_ + "\"used in script declaration \"" + scriptCalled + "\"");
		}
		scriptCalledName = scriptCalled.substr(namespaceSeparatorPosition + 1);
	} else {
		scriptCalledName = scriptCalled;
	}

	switch (strategy) {
	case ska::ScriptNameStrategy::WORKING_DIRECTORY:
		SLOG_STATIC(LogLevel::Debug, ska::ScriptNameDeduceTag) << "Script name deducing strategy WORKING_DIRECTORY for script " << scriptCalled;
		return ska::ScriptNameCurrentWorkingDirectory(scriptCalledName);

	case ScriptNameStrategy::RELATIVE_TO_RUNNER:
		SLOG_STATIC(LogLevel::Debug, ska::ScriptNameDeduceTag) << "Script name deducing strategy RELATIVE_TO_RUNNER for script " << scriptCalled;
		return ska::ScriptNameRelativeToRunnerDirectory(scriptCalledName);

	case ScriptNameStrategy::STD_DIRECTORY:
		SLOG_STATIC(LogLevel::Debug, ska::ScriptNameDeduceTag) << "Script name deducing strategy STD_DIRECTORY for script " << scriptCalled;
		return ska::ScriptNameRelativeToStandardLibraryDirectory(scriptCalledName, stdPath);
	default:
		SLOG_STATIC(LogLevel::Debug, ska::ScriptNameDeduceTag) << "Script name deducing strategy PARENT_DIRECTORY for script " << scriptCalled;
		return ska::ScriptNameRelativeToParentDirectory(scriptCaller, scriptCalledName);
	}
}