#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include "../Script/ScriptComponent.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	class MemoryScript;
	class ScriptRunnerSystem;
	
	class ScriptUtils {
	private:
		ScriptUtils();
	
	public:
		static std::string getGlobalVariableKey(const std::string& v);
		static std::string getComponentVariableKey(const std::string& v);
		static std::string getValueFromVarOrSwitchNumber(const MemoryScript& saveGame, const ScriptComponent& script, std::string varNumber);
		static std::string replaceVariablesByNumerics(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& line);
		static std::string replaceVariablesByNumerics(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& line, char varStartSymbol, char varEndSymbol, const std::string& oldLine = "");
		static std::string getFirstExpressionFromLine(ScriptRunnerSystem& system, const std::string& line, ScriptComponent& script, size_t* outputCommandSize);
		static std::string getCommandCall(const std::string& s);
		static void setValueFromVarOrSwitchNumber(MemoryScript& saveGame, const std::string& scriptExtendedName, std::string varNumber, std::string value, std::unordered_map<std::string, std::string>& varMap);

		static std::string interpretVarName(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& v);
		static bool isScriptActivated(const MemoryScript& saveGame, const std::string& script);

		static std::optional<ska::ScriptSleepComponent> instantiateScript(const std::vector<std::string>& args, std::size_t startIndex, const ScriptComponent& script);

		~ScriptUtils() = default;
	
	private:
		static std::pair<std::size_t, std::size_t> extractFirstVariableNameBoundaries(const std::string& line, char varStartSymbol, char varEndSymbol);
	};
}
