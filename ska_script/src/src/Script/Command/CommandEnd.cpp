#include "CommandEnd.h"
#include "../ScriptUtils.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Data/MemoryScript.h"
#include "Core/Exceptions/ScriptDiedException.h"

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
