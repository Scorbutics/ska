#include <limits>
#include "ScriptRefreshSystem.h"
#include "Utils/FileUtils.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "ECS/EntityManager.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "ECS/Basics/Script/ScriptPositionedGetter.h"
#include "ECS/Basics/Physic/BlockAllowance.h"
#include "Data/Events/InputKeyEvent.h"
#include "Inputs/InputAction.h"
#include "Inputs/InputContextManager.h"

ska::ScriptRefreshSystem::ScriptRefreshSystem(EntityManager& entityManager, GameEventDispatcher& ged, ScriptAutoSystem& scriptAutoSystem, ScriptPositionedGetter& spg, BlockAllowance& world) :
	ScriptRefreshSystemBase(entityManager),
	ScriptPositionSystemAccess(entityManager),
	SubObserver<InputKeyEvent>(std::bind(&ScriptRefreshSystem::onKeyEvent, this, std::placeholders::_1), ged),
	m_scriptPositionedGetter(spg),
	m_world(world),
	m_scriptAutoSystem(scriptAutoSystem),
	m_action(false) {	
}

bool ska::ScriptRefreshSystem::onKeyEvent(InputKeyEvent& ike) {
	m_action = ike.icm.getActions()[DoAction];
	return true;
}

void ska::ScriptRefreshSystem::refresh(unsigned int ellapsedTime) {
	auto& components = ScriptRefreshSystemBase::m_componentAccessor;
	auto& componentsSP = ScriptPositionSystemAccess::m_componentAccessor;
	std::vector<EntityId> toDelete;

	const auto& processed = ScriptRefreshSystemBase::getEntities();
	for (const auto& entityId : processed) {
		const auto& pc = components.get<PositionComponent>(entityId);

		{
			const auto& subProcessed = ScriptPositionSystemAccess::getEntities();
			EntityId scriptEntity;
			for (auto targets : subProcessed) {
				auto& scriptData = componentsSP.get<ScriptSleepComponent>(targets);

				switch (scriptData.triggeringType) {
				case ScriptTriggerType::AUTO:
					startScript(targets, entityId);
					toDelete.push_back(targets);
					break;

				case ScriptTriggerType::ACTION:
					if (!m_action) {
						break;
					}

				case ScriptTriggerType::MOVE_IN:
					scriptEntity = findNearScriptComponentEntity(pc, targets);
					if (scriptEntity != std::numeric_limits<unsigned int>::max()) {
						startScript(scriptEntity, entityId);
					}
					break;
				default:
					break;
				}

			}
		}

		/* TileWorld based events */
		const auto worldScripts = refreshScripts(entityId);
		for (const auto& ssc : worldScripts) {
			if (ssc != nullptr) {
				auto scriptCreated = ScriptRefreshSystemBase::createEntity();
				components.add<PositionComponent>(scriptCreated, PositionComponent(pc));
				components.add<ScriptSleepComponent>(scriptCreated, ScriptSleepComponent(*ssc));
				componentsSP.get<ScriptSleepComponent>(scriptCreated).deleteEntityWhenFinished = true;
				startScript(scriptCreated, entityId);
				if (ssc->triggeringType == ScriptTriggerType::AUTO) {
					toDelete.push_back(scriptCreated);
				}
			}
		}

	}

	for (const auto& targets : toDelete) {
		components.remove<ScriptSleepComponent>(targets);
	}


	m_scriptAutoSystem.update(ellapsedTime);

}

std::vector<ska::ScriptSleepComponent*> ska::ScriptRefreshSystem::refreshScripts(const ska::EntityId& entityId) {
	auto& components = ScriptRefreshSystemBase::m_componentAccessor;
	auto& componentsPossible = ScriptRefreshSystemBase::m_componentPossibleAccessor;

	const auto& pc = components.get<PositionComponent>(entityId);
	auto& sac = components.get<ScriptAwareComponent>(entityId);
	const auto& hc = components.get<HitboxComponent>(entityId);
	const auto& dac = components.get<AnimationComponent>(entityId);
	const Point<int>& frontPos = PositionComponent::getFrontPosition(pc, hc, dac);
	const Point<int>& centerPos = PositionComponent::getCenterPosition(pc, hc);
	
	const auto blockSize = m_world.getBlockSize();
	const auto oldCenterPos = Point<int>(sac.lastBlockPos);

	auto worldScripts = std::vector<ScriptSleepComponent*>{};
	auto autosScript = m_scriptPositionedGetter.getScriptsAuto();
	for (auto& autoScript : autosScript) {
		worldScripts.push_back(&autoScript.get());
	}

	if (m_action) {
		auto tmp = m_scriptPositionedGetter.getScripts(oldCenterPos, frontPos, ScriptTriggerType::ACTION);
		for (auto& script : tmp) {
			worldScripts.push_back(&script.get());
		}
	}

	//TODO revoir le système de détection de changement de bloc : par events
	const auto& wccPtr = componentsPossible.get<WorldCollisionComponent>(entityId);
	if (wccPtr != nullptr) {
		const auto& wcc = *wccPtr;
		if ((wcc.blockColPosX != wcc.lastBlockColPosX && wcc.blockColPosX != wcc.lastBlockColPosY) ||
			(wcc.blockColPosY != wcc.lastBlockColPosY && wcc.blockColPosY != wcc.lastBlockColPosX)) {

			auto tmp = m_scriptPositionedGetter.getScripts(oldCenterPos, frontPos, ScriptTriggerType::TOUCH);
			SKA_DBG_ONLY(
				if (!tmp.empty()) {
					SKA_LOG_DEBUG("Chipset script TOUCH");
				}
			);
			for (auto& script : tmp) {
				worldScripts.push_back(&script.get());
			}
		}
	}

	/* If we are moving to another block, triggers a MOVE_OUT event on previous block and MOVE_IN on the next one */
	const auto sameBlockBot = m_world.isSameBlockId(centerPos, oldCenterPos, 0);
	const auto sameBlockMid = m_world.isSameBlockId(centerPos, oldCenterPos, 1);
	if (!sameBlockBot || !sameBlockMid) {
		auto tmpOut = m_scriptPositionedGetter.getScripts(oldCenterPos, centerPos, ScriptTriggerType::MOVE_OUT);
		SKA_DBG_ONLY(
			if (!tmpOut.empty()) {
				SKA_LOG_DEBUG("Chipset script MOVE_OUT");
			}
		);
		for (auto& script : tmpOut) {
			worldScripts.push_back(&script.get());
		}

		auto tmpIn = m_scriptPositionedGetter.getScripts(oldCenterPos, centerPos, ScriptTriggerType::MOVE_IN);
		SKA_DBG_ONLY(
			if (!tmpIn.empty()) {
				SKA_LOG_DEBUG("Chipset script MOVE_IN");
			}
		);
		for (auto& script : tmpIn) {
			worldScripts.push_back(&script.get());
		}
	}

	if (oldCenterPos / blockSize != centerPos / blockSize) {
		sac.lastBlockPos.x = centerPos.x;
		sac.lastBlockPos.y = centerPos.y;
	}

	return worldScripts;
}

void ska::ScriptRefreshSystem::update(unsigned int ellapsedTime) {
	ScriptRefreshSystemBase::update(ellapsedTime);
}

void ska::ScriptRefreshSystem::registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity) {
	m_scriptAutoSystem.registerNamedScriptedEntity(nameEntity, entity);
}

void ska::ScriptRefreshSystem::clearNamedScriptedEntities() {
	m_scriptAutoSystem.clearNamedScriptedEntities();
}

void ska::ScriptRefreshSystem::startScript(const EntityId scriptEntity, const EntityId origin) {
	m_scriptAutoSystem.registerScript(nullptr, scriptEntity, origin);
}

ska::EntityId ska::ScriptRefreshSystem::findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script) const {
	const auto blockSizeSquared = m_world.getBlockSize() * m_world.getBlockSize();
	auto& componentsSp = ScriptPositionSystemAccess::m_componentAccessor;

	auto& scriptPos = componentsSp.get<PositionComponent>(script);

	const int varX = (scriptPos.x - entityPos.x);
	const int varY = (scriptPos.y - entityPos.y);
	const int varZ = (scriptPos.z - entityPos.z);
	const unsigned int distanceSquared = varX * varX + varY * varY + varZ * varZ;
	if (distanceSquared < blockSizeSquared) {
		return script;
	}

	return std::numeric_limits<unsigned int>::max();
}
