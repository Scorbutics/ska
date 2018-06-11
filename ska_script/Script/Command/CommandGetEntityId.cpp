#include <iostream>
#include "ECS/EntityLocator.h"
#include "CommandGetEntityId.h"
#include "Utils/StringUtils.h"
#include "../ScriptComponent.h"

ska::CommandGetEntityId::CommandGetEntityId(EntityManager& entityManager, const EntityLocator& locator) :
	AbstractFunctionCommand(entityManager),
	m_locator(locator) {
}

std::string ska::CommandGetEntityId::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	const auto& name = args[0];
	if (name == ".") {
		return StringUtils::toString(script.getOrigin());
	}
	return StringUtils::toString(m_locator.getEntityId(name));
}

int ska::CommandGetEntityId::argumentsNumber() {
	return 1;
}
