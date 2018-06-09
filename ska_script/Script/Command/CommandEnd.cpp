#include "CommandEnd.h"
#include "../ScriptUtils.h"
#include "Utils/StringUtils.h"
#include "Data/MemoryScript.h"
#include "Exceptions/ScriptDiedException.h"

ska::CommandEnd::CommandEnd(EntityManager& e) : 
	AbstractFunctionCommand(e) {
}

int ska::CommandEnd::argumentsNumber() {
	return 0;
}

const std::string& ska::CommandEnd::getCmdName() {
	static const std::string cmdName = "end";
	return cmdName;
}

std::string ska::CommandEnd::execute(ScriptComponent&, MemoryScript&, const std::vector<std::string>&) {
	throw ScriptDiedException("");
}
