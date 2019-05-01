#include <iostream>
#include "Data/MemoryScript.h"
#include "CommandLog.h"
#include "Logging/Logger.h"
#include "../ScriptUtils.h"

ska::CommandLog::CommandLog(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

std::string ska::CommandLog::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	//SKA_LOG_MESSAGE(ScriptUtils::replaceVariablesByNumerics(memory, script, args[0]));
	return "";
}

int ska::CommandLog::argumentsNumber() {
	return 1;
}

ska::CommandLog::~CommandLog()
{
}
