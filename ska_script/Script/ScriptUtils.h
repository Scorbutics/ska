#pragma once
#include <string>
#include <unordered_map>
#include "../Script/ScriptComponent.h"


namespace ska {
	class MemoryScript;
	class ScriptAutoSystem;
	
	class ScriptUtils {
	private:
		ScriptUtils();
	
	public:
		static std::string getGlobalVariableKey(const std::string& v);
		static std::string getComponentVariableKey(const std::string& v);
		static std::string getValueFromVarOrSwitchNumber(const MemoryScript& saveGame, const ScriptComponent& script, std::string varNumber);
		static std::string replaceVariablesByNumerics(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& line);
		static std::string replaceVariablesByNumerics(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& line, char varStartSymbol, char varEndSymbol);
		static std::string getFirstExpressionFromLine(ScriptAutoSystem& system, const std::string& line, ScriptComponent& script, size_t* outputCommandSize);
		static std::string getCommandCall(const std::string& s);
		static std::string getLocalVariableKey(const std::string& v);
		static void setValueFromVarOrSwitchNumber(MemoryScript& saveGame, const std::string& scriptExtendedName, std::string varNumber, std::string value, std::unordered_map<std::string, std::string>& varMap);

		static std::string interpretVarName(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& v);
		static bool isScriptActivated(const MemoryScript& saveGame, const std::string& script);
		~ScriptUtils() = default;
	};
}
