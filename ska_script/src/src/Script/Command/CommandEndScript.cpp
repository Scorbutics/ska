#include "CommandEndScript.h"
#include "Core/Data/MemoryScript.h"
#include "Core/Exceptions/ScriptDiedException.h"

ska::CommandEndScript::CommandEndScript(EntityManager& e) : 
	CommandEnd(e) {
}

int ska::CommandEndScript::argumentsNumber() {
	return 1;
}

std::string ska::CommandEndScript::execute(ScriptComponent&, MemoryScript&, const std::vector<std::string>& args) {
	throw ScriptDiedException(args[0]);
}
