#include <limits>
#include "ScriptRefreshSystem.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "ECS/EntityManager.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "ECS/Basics/Script/ScriptPositionedGetter.h"
#include "Data/Events/InputKeyEvent.h"
#include "Inputs/InputAction.h"
#include "Inputs/InputContextManager.h"
#include "Data/Events/ScriptEvent.h"

ska::ScriptRefreshSystem::ScriptRefreshSystem(EntityManager& entityManager, GameEventDispatcher& ged, ScriptAutoSystem& scriptAutoSystem, ScriptPositionedGetter& spg) :
	ScriptRefreshSystemBase(entityManager),
	ScriptPositionSystemAccess(entityManager),
	SubObserver<InputKeyEvent>(std::bind(&ScriptRefreshSystem::onKeyEvent, this, std::placeholders::_1), ged),
	SubObserver<CollisionEvent>(std::bind(&ScriptRefreshSystem::onCollisionEvent, this, std::placeholders::_1), ged),
	SubObserver<ScriptEvent>(std::bind(&ScriptRefreshSystem::onScriptEvent, this, std::placeholders::_1), ged),
	m_scriptPositionedGetter(spg),	
	m_scriptAutoSystem(scriptAutoSystem),
	m_action(false),
	m_entityManager(entityManager) {
}

bool ska::ScriptRefreshSystem::onKeyEvent(InputKeyEvent& ike) {
	m_action = ike.icm.getActions()[DoAction];
	if (m_action) {
		auto& components = ScriptRefreshSystemBase::m_componentAccessor;

		const auto& processed = ScriptRefreshSystemBase::getEntities();
		for (const auto& entityId : processed) {
			const auto& pc = components.get<PositionComponent>(entityId);
			auto& sac = components.get<ScriptAwareComponent>(entityId);
			const auto& hc = components.get<HitboxComponent>(entityId);
			const auto& dac = components.get<AnimationComponent>(entityId);
			
			const Point<int>& frontPos = PositionComponent::getFrontPosition(pc, hc, dac);
			const auto oldCenterPos = Point<int>(sac.lastBlockPos);

			auto tmp = m_scriptPositionedGetter.getScripts(oldCenterPos, frontPos, ScriptTriggerType::ACTION);
			
			auto worldScripts = std::vector<ScriptSleepComponent*>{};
			for (auto& script : tmp) {
				worldScripts.push_back(&script.get());
			}
			createScriptFromSleeping(worldScripts, entityId);
		}
		
	}

	return true;
}

bool ska::ScriptRefreshSystem::onCollisionEvent(CollisionEvent& ce) {
	//Filter only world collision
	if (ce.wcollisionComponent != nullptr) {
		const auto entityId = ce.entity;
	
		if (!m_entityManager.hasComponent<ScriptAwareComponent>(entityId)) {
			return false;
		}

		auto& sac = m_entityManager.getComponent<ScriptAwareComponent>(entityId);

		const auto oldCenterPos = Point<int>(sac.lastBlockPos);

		auto worldScripts = std::vector<ScriptSleepComponent*>{};

		for (const auto& frontPos : ce.wcollisionComponent->blockContacts) {
			auto tmp = m_scriptPositionedGetter.getScripts(oldCenterPos, frontPos, ScriptTriggerType::TOUCH, &ce.wcollisionComponent->normal);
			SKA_DEBUG_ONLY(
				if (!tmp.empty()) {
					SKA_LOG_DEBUG("Chipset script TOUCH");
				}
			);
			for (auto& script : tmp) {
				worldScripts.push_back(&script.get());
			}
		}
		createScriptFromSleeping(worldScripts, entityId);
	}
	return true;
}

void ska::ScriptRefreshSystem::triggerChangeBlockScripts(const ScriptEvent& se) {
	const auto entityId = se.entityId;
	auto worldScripts = std::vector<ScriptSleepComponent*>{};

	auto tmpOut = m_scriptPositionedGetter.getScripts(se.oldPos, se.newPos, ScriptTriggerType::MOVE_OUT);
	SKA_DEBUG_ONLY(
		if (!tmpOut.empty()) {
			SKA_LOG_DEBUG("Chipset script MOVE_OUT");
		}
	);
	for (auto& script : tmpOut) {
		worldScripts.push_back(&script.get());
	}

	auto tmpIn = m_scriptPositionedGetter.getScripts(se.oldPos, se.newPos, ScriptTriggerType::MOVE_IN);
	SKA_DEBUG_ONLY(
		if (!tmpIn.empty()) {
			SKA_LOG_DEBUG("Chipset script MOVE_IN");
		}
	);
	for (auto& script : tmpIn) {
		worldScripts.push_back(&script.get());
	}

	createScriptFromSleeping(worldScripts, entityId);
}

bool ska::ScriptRefreshSystem::onScriptEvent(ScriptEvent& se) {
	auto& componentsSP = ScriptPositionSystemAccess::m_componentAccessor;
	for (const auto& type : se.type) {
		switch (type) {
		case ScriptEventType::EntityChangeBlockProperty:
		case ScriptEventType::EntityChangeBlockId:
			triggerChangeBlockScripts(se);
			break;

		case ScriptEventType::ScriptCreate:
			if (startScript(se.entityId, se.parent)) {
				componentsSP.remove<ScriptSleepComponent>(se.entityId);
			}
			break;

		default:
			break;
		}
	}
	return true;
}

void ska::ScriptRefreshSystem::createScriptFromSleeping(const std::vector<ScriptSleepComponent*>& sleepings, const EntityId& parent) {
	auto& components = ScriptRefreshSystemBase::m_componentAccessor;
	auto& componentsSP = ScriptPositionSystemAccess::m_componentAccessor;

	const auto& pc = components.get<PositionComponent>(parent);
	std::vector<EntityId> toDelete;
	for (const auto& ssc : sleepings) {
		if (ssc != nullptr) {
			auto scriptCreated = ScriptRefreshSystemBase::createEntity();
			components.add<PositionComponent>(scriptCreated, PositionComponent(pc));
			componentsSP.add<ScriptSleepComponent>(scriptCreated, ScriptSleepComponent(*ssc));
			auto& script = componentsSP.get<ScriptSleepComponent>(scriptCreated);
			script.deleteEntityWhenFinished = true;
			if(startScript(scriptCreated, parent)) {
				toDelete.push_back(scriptCreated);
			}
		}
	}

	for (const auto& targets : toDelete) {
		componentsSP.remove<ScriptSleepComponent>(targets);
	}
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

	}

	for (const auto& targets : toDelete) {
		components.remove<ScriptSleepComponent>(targets);
	}


	m_scriptAutoSystem.update(ellapsedTime);

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

bool ska::ScriptRefreshSystem::startScript(const EntityId scriptEntity, const EntityId origin) {
	m_scriptAutoSystem.registerScript(nullptr, scriptEntity, origin);

	//Auto scripts are "one shots"	
	auto& componentsSp = ScriptPositionSystemAccess::m_componentAccessor;
	auto& ssc = componentsSp.get<ScriptSleepComponent>(scriptEntity);
	return ssc.triggeringType == ScriptTriggerType::AUTO;
}

ska::EntityId ska::ScriptRefreshSystem::findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script, const unsigned int distance) const {
	auto& componentsSp = ScriptPositionSystemAccess::m_componentAccessor;

	auto& scriptPos = componentsSp.get<PositionComponent>(script);

	const int varX = (scriptPos.x - entityPos.x);
	const int varY = (scriptPos.y - entityPos.y);
	const int varZ = (scriptPos.z - entityPos.z);
	const unsigned int effectiveDistanceSquared = varX * varX + varY * varY + varZ * varZ;
	if (effectiveDistanceSquared < (distance * distance)) {
		return script;
	}

	return std::numeric_limits<unsigned int>::max();
}
