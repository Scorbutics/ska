#include "CommandAssignSwitch.h"
#include "Data/MemoryScript.h"
#include "../ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"

ska::CommandAssignSwitch::CommandAssignSwitch(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandAssignSwitch::~CommandAssignSwitch()
{
}

int ska::CommandAssignSwitch::argumentsNumber() {
	return 2;
}

std::string ska::CommandAssignSwitch::execute(ScriptComponent& script, MemoryScript& memory, std::vector<std::string>& args) {
	const auto varValue = args[0];
	const auto varNumber = args[1];

	//TODO virer ScriptUtils & le getSavegame, faire un service
	ScriptUtils::setValueFromVarOrSwitchNumber(memory, script.extendedName, ScriptSymbolsConstants::SWITCH_LEFT + varNumber + ScriptSymbolsConstants::SWITCH_RIGHT, varValue, script.varMap);
	return "";
}
