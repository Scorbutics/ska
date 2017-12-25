#include <sstream>
#include "ScriptUtils.h"
#include "Utils/FormalCalculation/FormalCalculator.h"
#include "../Script/ScriptSymbolsConstants.h"
#include "Utils/StringUtils.h"
#include "Utils/SkaConstants.h"
#include "../Script/System/ScriptAutoSystem.h"
#include "Exceptions/ScriptSyntaxError.h"
#include "Exceptions/NumberFormatException.h"

ska::ScriptUtils::ScriptUtils()
{
}

std::string ska::ScriptUtils::getGlobalVariableKey(const std::string& v) {
	const auto pipePos = v.find_first_of('$');
	if (pipePos == 0 && v.find_last_of('$') == v.size() - 1) {
		//variable globale
		return v.substr(1, v.size() - 2);
	}

	return "";
}

/* Récupère la valeur une variable LOCALE (dans varMap) */
std::string ska::ScriptUtils::getValueFromVarOrSwitchNumber(const Savegame& saveGame, const ScriptComponent& script, std::string varNumber) {
	
	if (varNumber[0] == '{' && varNumber[varNumber.size() - 1] == '}') {
		if (saveGame.getGameSwitch(varNumber.substr(1, varNumber.size() - 2))) {
			return "1";
		} else {
			return "0";
		}
	} else if (varNumber[0] == '[' && varNumber[varNumber.size() - 1] == ']') {
		const auto& formattedVarNumber = varNumber.substr(1, varNumber.size() - 2);
		auto keyLocal = getLocalVariableKey(formattedVarNumber);
		if (!keyLocal.empty()) {
			if (script.varMap.find(keyLocal) != script.varMap.end()) {
				return script.varMap.at(keyLocal);
			}
			return formattedVarNumber;
		}

		auto keyGlobal = getGlobalVariableKey(formattedVarNumber);
		if (!keyGlobal.empty()) {
			return StringUtils::intToStr(saveGame.getGameVariable(keyGlobal));
		}

		const auto lastVarName = interpretVarName(saveGame, script, formattedVarNumber);
		return lastVarName;
	} else if (varNumber[0] == '#' && varNumber[varNumber.size() - 1] == '#') {
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

std::string ska::ScriptUtils::replaceVariablesByNumerics(const Savegame& saveGame, const ScriptComponent& script, const std::string& line, char varStartSymbol, char varEndSymbol)
{
	auto it = line;
	size_t posLeft, posRight;

	while ((posLeft = it.find_first_of(varStartSymbol)) != std::string::npos)
	{
		if ((posRight = it.substr(posLeft + 1).find_first_of(varEndSymbol)) != std::string::npos)
		{
			posRight += posLeft + 1;

			const auto var = it.substr(posLeft, posRight - posLeft + 1);
			const auto varValue = getValueFromVarOrSwitchNumber(saveGame, script, var);

			it = it.substr(0, posLeft) + varValue + it.substr(posRight + 1, it.size());
		}
		else
		{
			FormalCalculator::calculSyntaxError(line);
			return "";
		}
	}

	return it;

}

std::string ska::ScriptUtils::replaceVariablesByNumerics(const Savegame& saveGame, const ScriptComponent& script, const std::string& line)
{
	std::string it = replaceVariablesByNumerics(saveGame, script, line, ScriptSymbolsConstants::VARIABLE_LEFT, ScriptSymbolsConstants::VARIABLE_RIGHT);
	it = replaceVariablesByNumerics(saveGame, script, it, ScriptSymbolsConstants::ARG, ScriptSymbolsConstants::ARG);
	it = replaceVariablesByNumerics(saveGame, script, it, ScriptSymbolsConstants::SWITCH_LEFT, ScriptSymbolsConstants::SWITCH_RIGHT);
	return it;
}

/*
Retourne la première expression sur "line" après calculs et formattage
Ex : si line = " [|bidule|] %random 100% [|chance|] %truc 200% " , la fonction va calculer "random 100" et renvoyer le résultat de ce calcul.
Ex : si line = " [|bidule|] %random %truc 200%% [|chance|]" , la fonction va calculer "random %truc 200%", lui-même va rappeler cette fonction et renvoyer le résultat de ce calcul total.
*/
std::string ska::ScriptUtils::getFirstExpressionFromLine(ScriptAutoSystem& system, const std::string& line, ScriptComponent& script, size_t* outputCommandSize)
{
	size_t indexFirstChar;
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

std::string ska::ScriptUtils::getLocalVariableKey(const std::string& v)
{
	const auto pipePos = v.find_first_of('|');
	if (pipePos == 0 && v.find_last_of('|') == v.size() - 1)
	{
		//variable temporaire => varMap
		auto key = "[" + v.substr(1, v.size() - 2) + "]";
		return key;
	}

	return "";
}

void ska::ScriptUtils::setValueFromVarOrSwitchNumber(Savegame& saveGame, const std::string& scriptExtendedName, std::string varNumber, std::string value, std::unordered_map<std::string, std::string>& varMap) {
	if (value.empty())
		return;

	if (varNumber[0] == '{' && varNumber[varNumber.size() - 1] == '}') {
		saveGame.setGameSwitch(varNumber.substr(1, varNumber.size() - 2), (value == "1"));
	} else if (varNumber[0] == '[' && varNumber[varNumber.size() - 1] == ']') {
		const auto v = varNumber.substr(1, varNumber.size() - 2);
		auto key = getLocalVariableKey(v);
		if (!key.empty()) {
			varMap[key] = value;
			return;
		}

		auto keyGlobal = getGlobalVariableKey(v);
		if(!keyGlobal.empty()) {
			saveGame.setGameVariable(keyGlobal, ska::StringUtils::strToInt(value));
		}

	} else if (varNumber[0] == '#' && varNumber[varNumber.size() - 1] == '#') {
		varMap[varNumber + scriptExtendedName] = value;
	}

}

/* Récupère la valeur d'une variable GLOBALE en utilisant potentiellement des sous-variables locales en paramètres */
std::string ska::ScriptUtils::interpretVarName(const Savegame& saveGame, const ScriptComponent& script, const std::string& v)
{
	/*
	$variable$   : variable globale à tout le jeu
	_variable_ : variable "constante" (intégrée au jeu)
	_variable param1 param2_ : variable intégrée au jeu avec paramètres
	#variable# : numéro d'argument de script en cours entre symboles dièse
	|variable| : variable utilisateur (créée en script et utilisée en script, morte à la fin du script)
	*/

	std::stringstream ss;
	std::string cmds[2];

	if (v[0] == '_' && v[v.size()-1] == '_')
	{
		ss << v.substr(1, v.size()-2);

		for (unsigned int i = 0; ss >> cmds[i] && i <= 2; i++);

		if (!ss.eof()) {
			throw ScriptSyntaxError("Error while interpreting global var (not enough arguments) : " + v);
		}

		return script.parent->map(cmds[0], getValueFromVarOrSwitchNumber(saveGame, script, cmds[1]));
	}

	return getValueFromVarOrSwitchNumber(saveGame, script, v);
}

bool ska::ScriptUtils::isScriptActivated(const Savegame& saveGame, const std::string& scriptName)
{
	std::string s;
	std::ifstream scriptList(("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + saveGame.getSaveName() + FILE_SEPARATOR "scripts.data").c_str(), std::ios::app);
	std::ifstream tmpScriptList(("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + saveGame.getSaveName() + FILE_SEPARATOR "tmpscripts.data").c_str(), std::ios::app);

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

ska::ScriptUtils::~ScriptUtils()
{
}
