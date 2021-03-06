#include "Script/__internalConfig/LoggerConfig.h"
#include "CommandBlockAuthorize.h"
#include "Core/Data/MemoryScript.h"
#include "../ScriptUtils.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/Exceptions/ScriptException.h"
#include "Core/ECS/EntityManager.h"

ska::CommandBlockAuthorize::CommandBlockAuthorize(EntityManager& entityManager) : 
	AbstractFunctionCommand(entityManager) {
}

int ska::CommandBlockAuthorize::argumentsNumber() {
	return 3;
}

std::string ska::CommandBlockAuthorize::execute(ScriptComponent&, MemoryScript& memory, const std::vector<std::string>& args) {
	auto blockId = StringUtils::strToInt(args[0]);
	auto entity = StringUtils::strToInt(args[1]);
	auto unauth = args[2] == "1";

	if (!m_entityManager.hasComponent<CollidableComponent>(entity)) {
		//TODO Mess
		throw ScriptException("");
	}

	CollidableComponent& cc = m_entityManager.getComponent<CollidableComponent>(entity);

	auto it = cc.authorizedBlockIds.find(blockId);
	if (unauth && it != cc.authorizedBlockIds.end()) {
		cc.authorizedBlockIds.erase(it);
	} else {
		cc.authorizedBlockIds.emplace(blockId);
	}

	return "";
}
