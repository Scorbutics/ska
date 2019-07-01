#include "Script/__internalConfig/LoggerConfig.h"
#include "CommandCalculate.h"
#include "Core/Data/MemoryScript.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../ScriptUtils.h"
#include "Core/Utils/FormalCalculation/FormalCalculator.h"

ska::CommandCalculate::CommandCalculate(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int ska::CommandCalculate::argumentsNumber() {
	return 1;
}

std::string ska::CommandCalculate::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
    //TODO virer ScriptUtils & le getSavegame, faire un service
	float f = FormalCalculator::interpretFormalCalculation(ScriptUtils::replaceVariablesByNumerics(memory, script, args[0]));
	return StringUtils::intToStr((int)f);
}
