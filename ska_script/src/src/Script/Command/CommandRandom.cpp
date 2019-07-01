#include "Script/__internalConfig/LoggerConfig.h"
#include "CommandRandom.h"
#include "../ScriptUtils.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/Data/MemoryScript.h"
#include "Base/Values/Numbers/NumberUtils.h"

ska::CommandRandom::CommandRandom(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int ska::CommandRandom::argumentsNumber() {
	return 1;
}

std::string ska::CommandRandom::execute(ScriptComponent&, MemoryScript&, const std::vector<std::string>& args) {
	std::string s, value, commandCall;
	int valueInt, resultInt;

	value = args[0];
	valueInt = StringUtils::strToInt(value);

	if (valueInt > 0)
	{
		resultInt = NumberUtils::random(0, valueInt);
		return StringUtils::intToStr(resultInt);
	}
	return "";
}
