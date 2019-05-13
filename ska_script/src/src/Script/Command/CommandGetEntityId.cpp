#include <iostream>
#include "Script/__internalConfig/LoggerConfig.h"
#include "Core/ECS/EntityLocator.h"
#include "CommandGetEntityId.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../ScriptComponent.h"

ska::CommandGetEntityId::CommandGetEntityId(EntityManager& entityManager, const EntityLocator& locator) :
	AbstractFunctionCommand(entityManager),
	m_locator(locator) {
}

std::string ska::CommandGetEntityId::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	const auto& name = args[0];
	if (name == ".target") {
		return StringUtils::toString(script.getTarget());
	} else if (name == ".origin") {
		return StringUtils::toString(script.getOrigin());
	}
	
	auto locatedPtr = m_locator.getEntityId(name);
	if (locatedPtr != nullptr) {
		return StringUtils::toString(*locatedPtr);
	}
	return "";
}

int ska::CommandGetEntityId::argumentsNumber() {
	return 1;
}
