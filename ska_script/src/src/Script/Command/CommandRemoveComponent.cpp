#include "CommandRemoveComponent.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandRemoveComponent::CommandRemoveComponent(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int ska::CommandRemoveComponent::argumentsNumber() {
	return 2;
}

std::string ska::CommandRemoveComponent::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	const auto& componentName = args[0];
	const auto& entity = ska::StringUtils::fromString<ska::EntityId>(args[1]);

	//TODO event ?
	script.parent->removeComponent(componentName, entity);

	return "";
}
