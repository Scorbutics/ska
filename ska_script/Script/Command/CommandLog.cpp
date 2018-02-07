#include <iostream>
#include "CommandLog.h"
#include "Logging/Logger.h"

ska::CommandLog::CommandLog(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}

std::string ska::CommandLog::execute(ScriptComponent&, MemoryScript&, std::vector<std::string>& args) {
	SKA_LOG_MESSAGE(args[0]);
	return "";
}

int ska::CommandLog::argumentsNumber() {
	return 1;
}

ska::CommandLog::~CommandLog()
{
}
