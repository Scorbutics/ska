#include "CommandEndScript.h"
#include "Data/MemoryScript.h"
#include "Exceptions/ScriptDiedException.h"

ska::CommandEndScript::CommandEndScript(EntityManager& e) : 
	CommandEnd(e) {
}

int ska::CommandEndScript::argumentsNumber() {
	return 1;
}

std::string ska::CommandEndScript::execute(ScriptComponent&, MemoryScript&, const std::vector<std::string>& args) {
	throw ScriptDiedException(args[0]);
}
