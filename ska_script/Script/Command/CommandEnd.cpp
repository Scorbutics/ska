#include "CommandEnd.h"
#include "../ScriptUtils.h"
#include "Utils/StringUtils.h"
#include "Data/MemoryScript.h"
#include "Exceptions/ScriptDiedException.h"

ska::CommandEnd::CommandEnd(EntityManager& e) : AbstractFunctionCommand(e)
{
}


ska::CommandEnd::~CommandEnd()
{
}

int ska::CommandEnd::argumentsNumber() {
	return 0;
}

const std::string& ska::CommandEnd::getCmdName() {
	static const std::string cmdName = "end";
	return cmdName;
}

std::string ska::CommandEnd::execute(ScriptComponent&, MemoryScript&, std::vector<std::string>&) {
	throw ScriptDiedException("");
}
