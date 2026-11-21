#pragma once
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "Base/IO/Files/FileUtils.h"

#define SKALANG_SCRIPT_EXT "miniska"

namespace ska {

	enum class ScriptNameStrategy {
    	WORKING_DIRECTORY,
    	RELATIVE_TO_PARENT,
    	RELATIVE_TO_RUNNER,
    	STD_DIRECTORY
	};

	std::string& ScriptNameStandardLibraryPath();

	// "bind:<name>" scripts read from a file instead of a native binding (ex : a checker compiling scripts with the templates of the bindings)
	std::unordered_map<std::string, std::string>& ScriptNameBindingAliases();

	//Execution relative path (based on current working directory)
	inline std::string ScriptNameCurrentWorkingDirectory(const std::string& scriptCalled) {
		return FileUtils::getCanonicalPath(FileUtils::getCurrentDirectory() + "/" + scriptCalled + "." SKALANG_SCRIPT_EXT );
	}

	//Executable relative path
	inline std::string ScriptNameRelativeToRunnerDirectory(const std::string& scriptCalled) {
    	const auto fileNameData = FileNameData {FileUtils::getExecutablePath()};
    	return FileUtils::getCanonicalPath(fileNameData.path + "/" + scriptCalled + "." SKALANG_SCRIPT_EXT);
	}

	//Standard library relative path
	inline std::string ScriptNameRelativeToStandardLibraryDirectory(const std::string& scriptCalled, const std::string& stdPath = ScriptNameStandardLibraryPath()) {
    	return FileUtils::getCanonicalPath(stdPath + "/" + scriptCalled + "." SKALANG_SCRIPT_EXT);
	}

	//File relative path
	inline std::string ScriptNameRelativeToParentDirectory(const std::string& scriptCaller, const std::string& scriptCalled) {
		const auto fileNameData = FileNameData {scriptCaller};
    	if(fileNameData.path.empty() || fileNameData.name.empty()) {
        	const auto subfileNameData = FileNameData {FileUtils::getExecutablePath()};
        	return FileUtils::getCanonicalPath(subfileNameData.path + "/" + scriptCalled + "." SKALANG_SCRIPT_EXT);
    	}
    	return FileUtils::getCanonicalPath(fileNameData.path + "/" + scriptCalled + "." SKALANG_SCRIPT_EXT);
	}

	std::string ScriptNameDeduce(const std::string& scriptCaller, const std::string& scriptCalled, const std::string& stdPath = ScriptNameStandardLibraryPath());
}