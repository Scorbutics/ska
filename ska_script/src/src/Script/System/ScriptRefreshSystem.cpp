#include <limits>
#include "Script/__internalConfig/LoggerConfig.h"
#include "ScriptRefreshSystem.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Core/ECS/Basics/Script/ScriptTriggerType.h"
#include "Core/ECS/Basics/Script/ScriptPositionedGetter.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Core/Inputs/InputAction.h"
#include "Core/Inputs/InputContextManager.h"
#include "Core/Data/Events/ScriptEvent.h"

ska::ScriptRefreshSystem::ScriptRefreshSystem(EntityManager& entityManager, GameEventDispatcher& ged, ScriptRunnerSystem& scriptAutoSystem, ScriptPositionedGetter& spg) :
	ScriptRefreshSystemBase(entityManager),
	SubObserver<InputKeyEvent>(std::bind(&ScriptRefreshSystem::onKeyEvent, this, std::placeholders::_1), ged),
	SubObserver<CollisionEvent>(std::bind(&ScriptRefreshSystem::onCollisionEvent, this, std::placeholders::_1), ged),
	SubObserver<ScriptEvent>(std::bind(&ScriptRefreshSystem::onScriptEvent, this, std::placeholders::_1), ged),
	m_scriptPositionSystem(entityManager),
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

			auto tmp = std::move(m_scriptPositionedGetter.getScripts(oldCenterPos, frontPos, ScriptTriggerType::ACTION));
			
			auto worldScripts = std::vector<ScriptSleepComponent>{};
			for (auto& script : tmp) {
				worldScripts.push_back(std::move(script));
			}

			const auto& entities = m_scriptPositionSystem.getEntities();
			for (const auto& e : entities) {
				auto scriptEntity = findNearScriptComponentEntity(pc, e);
				if (scriptEntity.has_value()) {
					auto ssc = m_entityManager.getComponent<ska::ScriptSleepComponent>(*scriptEntity);
					startScript(std::move(ssc), e);
				}
			}

			createScriptFromSleeping(std::move(worldScripts), entityId);
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

		auto worldScripts = std::vector<ScriptSleepComponent>{};

		for (const auto& frontPos : ce.wcollisionComponent->blockContacts) {
			auto tmp = std::move(m_scriptPositionedGetter.getScripts(oldCenterPos, frontPos, ScriptTriggerType::TOUCH, &ce.wcollisionComponent->lastBlockDirection));
			if (!tmp.empty()) {
				SLOG(LogLevel::Debug) << "Chipset script TOUCH";
			}			
			for (auto& script : tmp) {
				worldScripts.push_back(std::move(script));
			}
		}
		createScriptFromSleeping(std::move(worldScripts), entityId);
	}
	return true;
}

void ska::ScriptRefreshSystem::triggerChangeBlockScripts(const ScriptEvent& se) {
	auto worldScripts = std::vector<ScriptSleepComponent>{};

	auto tmpOut = std::move(m_scriptPositionedGetter.getScripts(se.oldPos, se.newPos, ScriptTriggerType::MOVE_OUT));
	if (!tmpOut.empty()) {
		SLOG(LogLevel::Error) << "Chipset script MOVE_OUT";
	}
	
	for (auto& script : tmpOut) {
		worldScripts.push_back(std::move(script));
	}

	auto tmpIn = std::move(m_scriptPositionedGetter.getScripts(se.oldPos, se.newPos, ScriptTriggerType::MOVE_IN));
	if (!tmpIn.empty()) {
		SLOG(LogLevel::Error) << "Chipset script MOVE_IN";
	}
	
	for (auto& script : tmpIn) {
		worldScripts.push_back(std::move(script));
	}

	createScriptFromSleeping(std::move(worldScripts), se.parent);
}

bool ska::ScriptRefreshSystem::onScriptEvent(ScriptEvent& se) {
	for (const auto& type : se.type) {
		switch (type) {
		case ScriptEventType::EntityChangeBlockProperty:
		case ScriptEventType::EntityChangeBlockId:
			triggerChangeBlockScripts(se);
			break;

		case ScriptEventType::ScriptCreate:
			startScript(std::move(se.scriptSleep), se.parent);		
			break;

		default:
			break;
		}
	}
	return true;
}

void ska::ScriptRefreshSystem::createScriptFromSleeping(std::vector<ScriptSleepComponent> sleepings, const EntityId& parent) {
	//auto& components = ScriptRefreshSystemBase::m_componentAccessor;

	//const auto& pc = components.get<PositionComponent>(parent);
	//std::vector<EntityId> toDelete;
	for (auto& ssc : sleepings) {
		//auto scriptCreated = ScriptRefreshSystemBase::createEntity();
		//components.add<PositionComponent>(scriptCreated, PositionComponent(pc));
		//m_entityManager.addComponent<ScriptSleepComponent>(scriptCreated, std::move(ssc));
		//auto& script = m_entityManager.getComponent<ScriptSleepComponent>(scriptCreated);
		//script.deleteEntityWhenFinished = true;
		startScript(std::move(ssc), parent);
			//toDelete.push_back(scriptCreated);
				
	}

	/*for (const auto& targets : toDelete) {
		m_entityManager.removeComponent<ScriptSleepComponent>(targets);
	}*/
}

void ska::ScriptRefreshSystem::refresh(unsigned int ellapsedTime) {
	auto& components = ScriptRefreshSystemBase::m_componentAccessor;
	std::vector<EntityId> toDelete;

	const auto& processed = ScriptRefreshSystemBase::getEntities();
	for (const auto& entityId : processed) {
		const auto& pc = components.get<PositionComponent>(entityId);
		const auto& hc = components.get<HitboxComponent>(entityId);
		const auto& pos = PositionComponent::getCenterPosition(pc, hc);
		auto& sac = components.get<ScriptAwareComponent>(entityId);
		const auto oldCenterPos = Point<int>(sac.lastBlockPos);

		{
			auto tmp = std::move(m_scriptPositionedGetter.getScripts(oldCenterPos, pos, ScriptTriggerType::MOVE_IN));
			for (auto& script : tmp) {
				startScript(std::move(script), entityId);
			}
		}
		{
			auto tmp = m_scriptPositionedGetter.getScripts(oldCenterPos, pos, ScriptTriggerType::MOVE_OUT);
			for (auto& script : tmp) {
				startScript(std::move(script), entityId);	
			}
		}

		/*
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
		}*/

	}

	for (const auto& targets : toDelete) {
		components.remove<ScriptSleepComponent>(targets);
	}

	m_scriptAutoSystem.update(ellapsedTime);
}

void ska::ScriptRefreshSystem::update(unsigned int ellapsedTime) {
	ScriptRefreshSystemBase::update(ellapsedTime);
}

bool ska::ScriptRefreshSystem::startScript(ScriptSleepComponent script, const EntityId origin) {
	m_scriptAutoSystem.registerScript(script, origin);

	//Auto scripts are "one shots"	
	return script.triggeringType == ScriptTriggerType::AUTO;
}

std::optional<ska::EntityId> ska::ScriptRefreshSystem::findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script, const unsigned int distance) const {
	auto& scriptPos = m_componentAccessor.get<PositionComponent>(script);

	const int varX = (scriptPos.x - entityPos.x);
	const int varY = (scriptPos.y - entityPos.y);
	const int varZ = (scriptPos.z - entityPos.z);
	const unsigned int effectiveDistanceSquared = varX * varX + varY * varY + varZ * varZ;
	if (effectiveDistanceSquared < (distance * distance)) {
		return script;
	}

	return {};
}
