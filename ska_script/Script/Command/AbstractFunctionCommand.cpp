#include "AbstractFunctionCommand.h"
#include "Utils/StringUtils.h"
#include "Exceptions/ScriptSyntaxError.h"
#include "../ScriptSymbolsConstants.h"
#include "../ScriptComponent.h"

ska::AbstractFunctionCommand::AbstractFunctionCommand(EntityManager& entityManager) : 
	AbstractCommand(entityManager) {
}

std::string ska::AbstractFunctionCommand::process(ScriptComponent& script, MemoryScript& memory, std::stringstream&, const std::vector<std::string>& args) {
	int argNumber = argumentsNumber();
	if (argNumber != -1 && static_cast<std::size_t>(argNumber) != args.size()) {
		/* Syntax error */
		auto syntaxErrorMsg = std::string { "Syntax error with parameters : " };
		for (const auto& arg : args) {
			syntaxErrorMsg += arg + " ";
		}
		throw ScriptSyntaxError((syntaxErrorMsg + " (wrong arguments number ?)").c_str());
	}

	return execute(script, memory, args);
}

char ska::AbstractFunctionCommand::getSeparator() {
	return ScriptSymbolsConstants::ARGUMENT_SEPARATOR;
}

ska::AbstractFunctionCommand::~AbstractFunctionCommand()
{
}
