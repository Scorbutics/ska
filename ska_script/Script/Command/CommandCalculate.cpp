#include "CommandCalculate.h"
#include "Utils/StringUtils.h"
#include "../ScriptUtils.h"
#include "Utils/FormalCalculation/FormalCalculator.h"

ska::CommandCalculate::CommandCalculate(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandCalculate::~CommandCalculate()
{
}

int ska::CommandCalculate::argumentsNumber() {
	return 1;
}

std::string ska::CommandCalculate::execute(ScriptComponent& script, MemoryScript& memory, std::vector<std::string>& args) {
    //TODO virer ScriptUtils & le getSavegame, faire un service
	float f = FormalCalculator::interpretFormalCalculation(ScriptUtils::replaceVariablesByNumerics(memory, script, args[0]));
	return StringUtils::intToStr((int)f);
}
