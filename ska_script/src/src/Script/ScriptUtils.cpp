#include <fstream>
#include <sstream>
#include <cstddef>
#include "ScriptUtils.h"
#include "Core/Utils/FormalCalculation/FormalCalculator.h"
#include "../Script/ScriptSymbolsConstants.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../Script/System/ScriptAutoSystem.h"
#include "Core/Exceptions/ScriptSyntaxError.h"
#include "Core/Exceptions/NumberFormatException.h"

ska::ScriptUtils::ScriptUtils() {}

std::string ska::ScriptUtils::getValueFromVarOrSwitchNumber(const MemoryScript& saveGame, const ScriptComponent& script, std::string varNumber) {
	 if (varNumber[0] == ScriptSymbolsConstants::VARIABLE_LEFT && varNumber[varNumber.size() - 1] == ScriptSymbolsConstants::VARIABLE_RIGHT) {
		const auto& formattedVarNumber = varNumber.substr(1, varNumber.size() - 2);

		const auto keyGlobal = getGlobalVariableKey(formattedVarNumber);
		if (!keyGlobal.empty()) {
			return StringUtils::intToStr(saveGame.getGameVariable(keyGlobal));
		}

		auto keyComponent = getComponentVariableKey(formattedVarNumber);
		if (!keyComponent.empty()) {
			const auto r = keyComponent.find_first_of(' ');
			if (r != std::string::npos) {
				keyComponent = keyComponent.substr(0, r) + " " + getValueFromVarOrSwitchNumber(saveGame, script, keyComponent.substr(r + 1));
			}
			
			return saveGame.getComponentVariable(keyComponent);
		}

		if (!formattedVarNumber.empty() && script.varMap.find(formattedVarNumber) != script.varMap.end()) {			
			return script.varMap.at(formattedVarNumber);
		}

	} else if (varNumber[0] == ScriptSymbolsConstants::ARG && varNumber[varNumber.size() - 1] == ScriptSymbolsConstants::ARG) {
		const auto& key = varNumber + script.extendedName;
		if (script.varMap.find(key) != script.varMap.end()) {
			return script.varMap.at(key);
		} 
		return varNumber;
	}

	return varNumber;
}

std::pair<std::size_t, std::size_t> ska::ScriptUtils::extractFirstVariableNameBoundaries(const std::string& line, char varStartSymbol, char varEndSymbol) {
	std::size_t posLeft;
	std::size_t posRight;

	if((posLeft = line.find_first_of(varStartSymbol)) != std::string::npos) {
		if ((posRight = line.find_last_of(varEndSymbol)) != std::string::npos) {
			return std::make_pair(posLeft, posRight);
		}
		FormalCalculator::calculSyntaxError(line);
	}
	return std::make_pair(std::string::npos, std::string::npos);
}

std::string ska::ScriptUtils::replaceVariablesByNumerics(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& line, char varStartSymbol, char varEndSymbol, const std::string& oldLine) {
	auto validPos = true;
	auto it = line;
	while(validPos) {
		auto [posLeft, posRight] = extractFirstVariableNameBoundaries(it, varStartSymbol, varEndSymbol);
		validPos = (posLeft != std::string::npos && posRight != std::string::npos);
		if (validPos) {

			const auto var = it.substr(posLeft, posRight - posLeft + 1);
			const auto varValue = getValueFromVarOrSwitchNumber(saveGame, script, var);
			if (var == varValue) {
				it = it.substr(posRight + 1);
			} else {
				it = it.substr(0, posLeft) + varValue + it.substr(posRight + 1);
			}
		}
	}

	return it;

}

std::string ska::ScriptUtils::replaceVariablesByNumerics(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& line) {
	std::string it = replaceVariablesByNumerics(saveGame, script, line, ScriptSymbolsConstants::VARIABLE_LEFT, ScriptSymbolsConstants::VARIABLE_RIGHT);
	return replaceVariablesByNumerics(saveGame, script, it, ScriptSymbolsConstants::ARG, ScriptSymbolsConstants::ARG);
}

/*
Retourne la première expression sur "line" après calculs et formattage
Ex : si line = " [|bidule|] %random 100% [|chance|] %truc 200% " , la fonction va calculer "random 100" et renvoyer le résultat de ce calcul.
Ex : si line = " [|bidule|] %random %truc 200%% [|chance|]" , la fonction va calculer "random %truc 200%", lui-même va rappeler cette fonction et renvoyer le résultat de ce calcul total.
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

std::string ska::ScriptUtils::getComponentVariableKey(const std::string& v) {
	const auto pipePos = v.find_first_of(ScriptSymbolsConstants::COMPONENT_VARIABLE_LEFT);
	if (pipePos == 0 && v.find_last_of(ScriptSymbolsConstants::COMPONENT_VARIABLE_RIGHT) == v.size() - 1) {
		//Component variable
		return v.substr(1, v.size() - 2);
	}

	return "";
}

void ska::ScriptUtils::setValueFromVarOrSwitchNumber(MemoryScript& saveGame, const std::string& scriptExtendedName, std::string varNumber, std::string value, std::unordered_map<std::string, std::string>& varMap) {
	if (value.empty()) {
		return;
	}

	if (varNumber[0] == ScriptSymbolsConstants::VARIABLE_LEFT && varNumber[varNumber.size() - 1] == ScriptSymbolsConstants::VARIABLE_RIGHT) {
		const auto v = varNumber.substr(1, varNumber.size() - 2);

		auto keyGlobal = getGlobalVariableKey(v);
		if(!keyGlobal.empty()) {
			saveGame.setGameVariable(keyGlobal, ska::StringUtils::strToInt(value));
			return;
		}

		auto keyComponent = getComponentVariableKey(v);
		if (!keyComponent.empty()) {
			saveGame.setComponentVariable(keyComponent, value);
		}

		if (!v.empty()) {
			varMap[v] = value;
			return;
		}

	} else if (varNumber[0] == ScriptSymbolsConstants::ARG && varNumber[varNumber.size() - 1] == ScriptSymbolsConstants::ARG) {
		varMap[varNumber + scriptExtendedName] = value;
	}

}

/* Récupère la valeur d'une variable en utilisant potentiellement des sous-variables locales en paramètres */
std::string ska::ScriptUtils::interpretVarName(const MemoryScript& saveGame, const ScriptComponent& script, const std::string& v) {
	/*
	$variable$   : variable globale à tout le jeu
	#variable# : numéro d'argument de script en cours entre symboles dièse : #arg0# #arg1# #arg2# etc...
	|variable| : variable locale (créée en script et utilisée en script, morte à la fin du script)
	<variable param> : composant à sérialiser / désérializer, le paramètre est souvent l'id de l'entité propriétaire
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

std::optional<ska::ScriptSleepComponent> ska::ScriptUtils::instantiateScript(const std::vector<std::string>& args, std::size_t startIndex, const ScriptComponent& script) {
	if (args.size() > startIndex) {
		std::vector<std::string> extraArgs;

		const auto& scriptName = args[startIndex];
		/* Rebuild an argument string to be read by the new running script */
		for (unsigned int i = startIndex + 1; i < args.size(); i++) {
			extraArgs.push_back(args[i]);
		}
		ska::ScriptSleepComponent ssc;
		ssc.context = script.context;
		ssc.args = extraArgs;
		ssc.period = 2000;
		ssc.deleteEntityWhenFinished = true;
		ssc.triggeringType = ska::ScriptTriggerType::AUTO;
		ssc.name = scriptName;
		return ssc;
	}
	return { };
}

