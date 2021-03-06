#include "Script/__internalConfig/LoggerConfig.h"
#include "CommandRestoreComponent.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandRestoreComponent::CommandRestoreComponent(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int ska::CommandRestoreComponent::argumentsNumber() {
	return 2;
}

std::string ska::CommandRestoreComponent::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	const auto& componentName = args[0];
	const auto& entity = ska::StringUtils::fromString<ska::EntityId>(args[1]);

	//TODO event ?
	//script.parent->restoreComponent(componentName, entity);

	return "";
}
