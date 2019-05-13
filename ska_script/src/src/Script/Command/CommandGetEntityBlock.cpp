#include <iostream>
#include "Script/__internalConfig/LoggerConfig.h"
#include "Core/Data/MemoryScript.h"
#include "CommandGetEntityBlock.h"
#include "Logging/Logger.h"
#include "../ScriptUtils.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "World/TileWorld.h"

ska::CommandGetEntityBlock::CommandGetEntityBlock(TileWorld& w, EntityManager& entityManager) :
	AbstractFunctionCommand(entityManager),
	m_world(w) {
}

std::string ska::CommandGetEntityBlock::execute(ScriptComponent& script, MemoryScript& memory, const std::vector<std::string>& args) {
	const auto& coord = args[0];
	const auto& entityId = ska::StringUtils::fromString<unsigned int>(args[1]);

	auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(entityId);
	auto& pc = m_entityManager.getComponent<ska::PositionComponent>(entityId);
	const auto pos = PositionComponent::getCenterPosition(pc, hc);

	return ska::StringUtils::toString(coord == "x" ? (pos.x / m_world.getBlockSize()) : (pos.y / m_world.getBlockSize()));
}

int ska::CommandGetEntityBlock::argumentsNumber() {
	return 2;
}
