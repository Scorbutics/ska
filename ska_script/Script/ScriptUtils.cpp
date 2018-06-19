#include <fstream>
#include <sstream>
#include <cstddef>
#include "ScriptUtils.h"
#include "Utils/FormalCalculation/FormalCalculator.h"
#include "../Script/ScriptSymbolsConstants.h"
#include "Utils/StringUtils.h"
#include "../Script/System/ScriptAutoSystem.h"
#include "Exceptions/ScriptSyntaxError.h"

ska::ScriptUtils::ScriptUtils() {}

/* R�cup�re la valeur une variable LOCALE (dans varMap) */
std::string ska::ScriptUtils::getValueFromVarOrSwitchNumber(const MemoryScript& saveGame, const ScriptComponent& script, std::string varNumber) {
	 if (varNumber[0] == ScriptSymbolsConstants::VARIABLE_LEFT && varNumber[varNumber.size() - 1] == ScriptSymbolsConstants::VARIABLE_RIGHT) {
		const auto& formattedVarNumber = varNumber.substr(1, varNumber.size() - 2);
		const auto keyLocal = getLocalVariableKey(formattedVarNumber);
		if (!keyLocal.empty()) {
			if (script.varMap.find(keyLocal) != script.varMap.end()) {
				return script.varMap.at(keyLocal);
			}
			return formattedVarNumber;
		}

		const auto keyGlobal = getGlobalVariableKey(formattedVarNumber);
		if (!keyGlobal.empty()) {
			return StringUtils::intToStr(saveGame.getGameVariable(keyGlobal));
		}

		const auto keyComponent = getComponentVariableKey(formattedVarNumber);
		if (!keyComponent.empty()) {
			return saveGame.getComponentVariable(keyComponent);
		}

		const auto lastVarName = interpretVarName(saveGame, script, formattedVarNumber);
		return varNumber;
	} else if (varNumber[0] == ScriptSymbolsConstants::ARG && varNumber[varNumber.size() - 1] == ScriptSymbolsConstants::ARG) {
		const auto& key = varNumber + script.extendedName;
		if (script.varMap.find(key) != script.varMap.end()) {
			return script.varMap.at(key);
		} else {
			return varNumber;
		}
	} else if (varNumber == "true") {
		return "1";
	} else if (varNumber == "false") {
		return "0";
	}

	return varNumber;
}

std::string ska::ScriptUtils::replaceVariablesByNumerics(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& line, char varStartSymbol, char varEndSymbol) {
	auto it = line;
	std::size_t posLeft, posRight;

	while ((posLeft = it.find_first_of(varStartSymbol)) != std::string::npos) {
		if ((posRight = it.substr(posLeft + 1).find_first_of(varEndSymbol)) != std::string::npos) {
			posRight += posLeft + 1;

			const auto var = it.substr(posLeft, posRight - posLeft + 1);
			const auto varValue = getValueFromVarOrSwitchNumber(saveGame, script, var);

			it = it.substr(0, posLeft) + varValue + it.substr(posRight + 1, it.size());
		} else {
			FormalCalculator::calculSyntaxError(line);
			return "";
		}
	}

	return it;

}

std::string ska::ScriptUtils::replaceVariablesByNumerics(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& line) {
	std::string it = replaceVariablesByNumerics(saveGame, script, line, ScriptSymbolsConstants::VARIABLE_LEFT, ScriptSymbolsConstants::VARIABLE_RIGHT);
	return replaceVariablesByNumerics(saveGame, script, it, ScriptSymbolsConstants::ARG, ScriptSymbolsConstants::ARG);
}

/*
Retourne la premi�re expression sur "line" apr�s calculs et formattage
Ex : si line = " [|bidule|] %random 100% [|chance|] %truc 200% " , la fonction va calculer "random 100" et renvoyer le r�sultat de ce calcul.
Ex : si line = " [|bidule|] %random %truc 200%% [|chance|]" , la fonction va calculer "random %truc 200%", lui-m�me va rappeler cette fonction et renvoyer le r�sultat de ce calcul total.
*/
std::string ska::ScriptUtils::getFirstExpressionFromLine(ScriptAutoSystem& system, const std::string& line, ScriptComponent& script, std::size_t* outputCommandSize) {
	std::size_t indexFirstChar;
	for (indexFirstChar = 0; line[indexFirstChar] != ScriptSymbolsConstants::METHOD && line[indexFirstChar] != '\n' && indexFirstChar < line.size(); indexFirstChar++);

	if (line[indexFirstChar] == '\n' || line.size() <= indexFirstChar) {
		if (outputCommandSize != nullptr) {
			*outputCommandSize = 0;
		}
		return line;
	}


	const std::string& formattedLine = line.substr(indexFirstChar, line.size());
	const std::string& commandCall = getCommandCall(formattedLine);
	std::string valeur;

	if (outputCommandSize != nullptr) {
		if (!commandCall.empty()) {
			*outputCommandSize = commandCall.size() + indexFirstChar + 2; //2 = les deux symboles '%' de la commande
		} else {
			*outputCommandSize = 0;
		}
	}

	if (!commandCall.empty()) {
		std::string result = system.interpret(script, system.getSavegame(), commandCall);
		if (!result.empty()) {
			valeur = result;
		} else {
			valeur = formattedLine.substr(0, formattedLine.find_first_of(' '));
		}
	} else {
		valeur = formattedLine.substr(0, formattedLine.find_first_of(' '));
	}

	return valeur;
}

std::string ska::ScriptUtils::getCommandCall(const std::string& s)
{
	if (!s.empty() && s[0] == ScriptSymbolsConstants::METHOD)
	{
		const auto posEndSubCmd = s.find_last_of(ScriptSymbolsConstants::METHOD);
		if (posEndSubCmd != std::string::npos)
			return s.substr(1, posEndSubCmd - 1);
	}

	return "";
}

std::string ska::ScriptUtils::getGlobalVariableKey(const std::string& v) {
	const auto pipePos = v.find_first_of(ScriptSymbolsConstants::GLOBAL_VARIABLE);
	if (pipePos == 0 && v.find_last_of(ScriptSymbolsConstants::GLOBAL_VARIABLE) == v.size() - 1) {
		//variable globale
		return v.substr(1, v.size() - 2);
	}

	return "";
}

std::string ska::ScriptUtils::getLocalVariableKey(const std::string& v)
{
	const auto pipePos = v.find_first_of(ScriptSymbolsConstants::LOCAL_VARIABLE);
	if (pipePos == 0 && v.find_last_of(ScriptSymbolsConstants::LOCAL_VARIABLE) == v.size() - 1)
	{
		//variable temporaire => varMap
		return "[" + v.substr(1, v.size() - 2) + "]";
	}

	return "";
}

std::string ska::ScriptUtils::getComponentVariableKey(const std::string& v) {
	const auto pipePos = v.find_first_of(ScriptSymbolsConstants::COMPONENT_VARIABLE_LEFT);
	if (pipePos == 0 && v.find_last_of(ScriptSymbolsConstants::COMPONENT_VARIABLE_RIGHT) == v.size() - 1) {
		//Component variable
		return v.substr(1, v.size() - 2);
	}

	return "";
}

void ska::ScriptUtils::setValueFromVarOrSwitchNumber(MemoryScript& saveGame, const std::string& scriptExtendedName, std::string varNumber, std::string value, std::unordered_map<std::string, std::string>& varMap) {
	if (value.empty())
		return;

	if (varNumber[0] == ScriptSymbolsConstants::VARIABLE_LEFT && varNumber[varNumber.size() - 1] == ScriptSymbolsConstants::VARIABLE_RIGHT) {
		const auto v = varNumber.substr(1, varNumber.size() - 2);
		auto key = getLocalVariableKey(v);
		if (!key.empty()) {
			varMap[key] = value;
			return;
		}

		auto keyGlobal = getGlobalVariableKey(v);
		if(!keyGlobal.empty()) {
			saveGame.setGameVariable(keyGlobal, ska::StringUtils::strToInt(value));
			return;
		}

		auto keyComponent = getComponentVariableKey(v);
		if (!keyComponent.empty()) {
			saveGame.setComponentVariable(keyComponent, value);
		}

	} else if (varNumber[0] == ScriptSymbolsConstants::ARG && varNumber[varNumber.size() - 1] == ScriptSymbolsConstants::ARG) {
		varMap[varNumber + scriptExtendedName] = value;
	}

}

/* R�cup�re la valeur d'une variable en utilisant potentiellement des sous-variables locales en param�tres */
std::string ska::ScriptUtils::interpretVarName(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& v) {
	/*
	$variable$   : variable globale � tout le jeu
	#variable# : num�ro d'argument de script en cours entre symboles di�se : #arg0# #arg1# #arg2# etc...
	|variable| : variable locale (cr��e en script et utilis�e en script, morte � la fin du script)
	<variable param> : composant � s�rialiser / d�s�rializer, le param�tre est souvent l'id de l'entit� propri�taire
	*/

	return getValueFromVarOrSwitchNumber(saveGame, script, v);
}

bool ska::ScriptUtils::isScriptActivated(const MemoryScript& saveGame, const std::string& scriptName)
{
	std::string s;
	std::ifstream scriptList(("./Data/Saves/" + saveGame.getSaveName() + "/scripts.data").c_str(), std::ios::app);
	std::ifstream tmpScriptList(("./Data/Saves/" + saveGame.getSaveName() + "/tmpscripts.data").c_str(), std::ios::app);

	if (scriptList.fail())
		return true;

	do
	scriptList >> s;
	while (s != scriptName && !scriptList.eof());


	if (s == scriptName)
		return false;
	else
	{
		s = "";
		if (tmpScriptList.fail())
			return true;

		do
		tmpScriptList >> s;
		while (s != scriptName && !tmpScriptList.eof());

		return !(s == scriptName);
	}
}

