#include <string>
#include "Script/__internalConfig/LoggerConfig.h"
#include "CommandScript.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Exceptions/ScriptSyntaxError.h"
#include "../System/ScriptAutoSystem.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Core/ECS/EntityManager.h"

ska::CommandScript::CommandScript(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int ska::CommandScript::argumentsNumber() {
	return -1;
}

std::string ska::CommandScript::execute(ScriptComponent& script, MemoryScript&, const std::vector<std::string>& args) {
	std::string scriptName;
	std::vector<std::string> extraArgs;
	std::string periodStr;

	if (args.size() < 2) {
		throw ScriptSyntaxError("Syntax error with command \"script\" : this command needs at least 2 parameters.");
	}

	scriptName = args[0];
	unsigned int period = StringUtils::strToInt(args[1]);

	/* Rebuild an argument string to be read by the new running script */
	for (unsigned int i = 2; i < args.size(); i++) {
		extraArgs.push_back(args[i]);
	}

	//ska::IScript* started = script->getParent().addRunningScript(script, scriptName, script->getContext(), extraArgs, 0, &period);
	EntityId newScript = m_entityManager.createEntity();
	ScriptSleepComponent ssc;
	ssc.context = script.context;
	ssc.args = extraArgs;
	ssc.period = period;
	ssc.deleteEntityWhenFinished = true;
	ssc.triggeringType = ScriptTriggerType::AUTO;
	ssc.name = scriptName;
	m_entityManager.addComponent<ScriptSleepComponent>(newScript, std::move(ssc));

	script.parent->registerScript(std::move(ssc), script.getOrigin());
	return StringUtils::intToStr(newScript);

}
