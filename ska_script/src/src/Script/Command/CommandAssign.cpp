#include <string>
#include "Core/Data/MemoryScript.h"
#include "CommandAssign.h"
#include "../ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"

ska::CommandAssign::CommandAssign(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int ska::CommandAssign::argumentsNumber() {
	return 2;
}

std::string ska::CommandAssign::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	std::string valeur, varNumber, line, commandCall;

	valeur = args[0];
	varNumber = args[1];

    //TODO virer ScriptUtils & le getSavegame, faire un service
	ScriptUtils::setValueFromVarOrSwitchNumber(memory, script.extendedName, ScriptSymbolsConstants::VARIABLE_LEFT + varNumber + ScriptSymbolsConstants::VARIABLE_RIGHT, valeur, script.varMap);
	return "";
}
