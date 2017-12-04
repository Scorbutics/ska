#include "CommandRemoveComponent.h"
#include "../ScriptUtils.h"
#include "Utils/StringUtils.h"
#include "../System/ScriptAutoSystem.h"

ska::CommandRemoveComponent::CommandRemoveComponent(EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


ska::CommandRemoveComponent::~CommandRemoveComponent()
{
}

int ska::CommandRemoveComponent::argumentsNumber() {
	return 2;
}

std::string ska::CommandRemoveComponent::execute(ScriptComponent& script, std::vector<std::string>& args) {
	const auto& componentName = args[0];
	const auto& entity = args[1];

	script.parent->removeComponent(componentName, entity);

	return "";
}
