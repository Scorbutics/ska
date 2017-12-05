#include "CommandAssignSwitch.h"
#include "../ScriptUtils.h"
#include "../ScriptSymbolsConstants.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandAssignSwitch::CommandAssignSwitch(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandAssignSwitch::~CommandAssignSwitch()
{
}

int ska::CommandAssignSwitch::argumentsNumber() {
	return 2;
}

std::string ska::CommandAssignSwitch::execute(ScriptComponent& script, std::vector<std::string>& args) {
	const auto varValue = args[0];
	const auto varNumber = args[1];

	ScriptUtils::setValueFromVarOrSwitchNumber(script.parent->getSavegame(), script.extendedName, ScriptSymbolsConstants::SWITCH_LEFT + varNumber + ScriptSymbolsConstants::SWITCH_RIGHT, varValue, script.varMap);
	return "";
}
