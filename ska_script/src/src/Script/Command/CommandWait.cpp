#include "CommandWait.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Data/MemoryScript.h"
#include "../../Script/ScriptComponent.h"

ska::CommandWait::CommandWait(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int ska::CommandWait::argumentsNumber() {
	return 1;
}

std::string ska::CommandWait::execute(ScriptComponent& script, MemoryScript&, const std::vector<std::string>& args) {
	unsigned int duree = 0;
	duree = StringUtils::strToInt(args[0]);

	/* Delay the script : assign an uint value > 1 */
	script.active = duree;
	return "";
}
