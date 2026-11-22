#include <limits>
#include "Script/__internalConfig/LoggerConfig.h"
#include "ScriptRefreshSystem.h"
#include <cmath>
#include "Core/Utils/PhysicUtils.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Core/ECS/Basics/Script/ScriptTriggerType.h"
#include "Core/ECS/Basics/Script/ScriptPositionedGetter.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Core/Inputs/InputAction.h"
#include "Core/Inputs/InputContextManager.h"
#include "Core/Data/Events/ScriptEvent.h"

ska::ScriptRefreshSystem::ScriptRefreshSystem(EntityManager& entityManager, GameEventDispatcher& ged, ScriptPositionedGetter& spg) :
	ScriptRefreshSystemBase(entityManager),
	SubObserver<InputKeyEvent>(std::bind(&ScriptRefreshSystem::onKeyEvent, this, std::placeholders::_1), ged),
	SubObserver<CollisionEvent>(std::bind(&ScriptRefreshSystem::onCollisionEvent, this, std::placeholders::_1), ged),
	SubObserver<ScriptEvent>(std::bind(&ScriptRefreshSystem::onScriptEvent, this, std::placeholders::_1), ged),
	m_scriptPositionSystem(entityManager),
	m_scriptPositionedGetter(spg),	
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
			
			// The block in front of the entity (the direction it looks to), from the block of its center : a character or a
			// tile on it answers
			const auto blockSize = static_cast<int>(m_scriptPositionedGetter.getBlockSize());
			const auto center = PositionComponent::getCenterPosition(pc, hc);
			const auto look = PhysicUtils::getMovement(dac.state, 1.F);
			const auto lookVector = Point<float>::cartesian(look.power, look.angle);
			const auto frontBlock = Point<int>{ static_cast<int>(center.x) / blockSize + static_cast<int>(std::lround(lookVector.x)),
				static_cast<int>(center.y) / blockSize + static_cast<int>(std::lround(lookVector.y)) };
			const auto frontPos = Point<int>{ frontBlock.x * blockSize + blockSize / 2, frontBlock.y * blockSize + blockSize / 2 };
			const auto oldCenterPos = Point<int>(sac.lastBlockPos);

			auto tmp = std::move(m_scriptPositionedGetter.getScripts(oldCenterPos, frontPos, ScriptTriggerType::ACTION));
			
			auto worldScripts = std::vector<ScriptSleepComponent>{};
			for (auto& script : tmp) {
				worldScripts.push_back(std::move(script));
			}

			const auto& entities = m_scriptPositionSystem.getEntities();
			for (const auto& e : entities) {
				const auto& scriptPc = m_entityManager.getComponent<PositionComponent>(e);
				const auto scriptCenter = m_entityManager.hasComponent<HitboxComponent>(e) ?
					PositionComponent::getCenterPosition(scriptPc, m_entityManager.getComponent<HitboxComponent>(e)) :
					Point<float>{ static_cast<float>(scriptPc.x), static_cast<float>(scriptPc.y) };
				if (static_cast<int>(scriptCenter.x) / blockSize == frontBlock.x && static_cast<int>(scriptCenter.y) / blockSize == frontBlock.y) {
					auto ssc = m_entityManager.getComponent<ska::ScriptSleepComponent>(e);
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

bool ska::ScriptRefreshSystem::onScriptEvent(ScriptEvent& se) {
	for (const auto& type : se.type) {
		switch (type) {
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

		// Once when the entity enters a block (not at each frame while it stays on it : ex a door would teleport again
		// and again) : MOVE_OUT for the block it leaves, MOVE_IN for the block it enters
		const auto blockSize = static_cast<int>(m_scriptPositionedGetter.getBlockSize());
		const auto center = Point<int>{ static_cast<int>(pos.x), static_cast<int>(pos.y) };
		const auto block = Point<int>{ center.x / blockSize, center.y / blockSize };
		if (blockSize > 0 && block != sac.enteredBlock) {
			const auto left = sac.enteredBlock;
			sac.enteredBlock = block;
			const auto leftPos = Point<int>{ left.x * blockSize, left.y * blockSize };
			if (left.x >= 0 && left.y >= 0) {
				auto tmp = m_scriptPositionedGetter.getScripts(leftPos, center, ScriptTriggerType::MOVE_OUT);
				for (auto& script : tmp) {
					startScript(std::move(script), entityId);
				}
			}
			auto tmp = m_scriptPositionedGetter.getScripts(left.x >= 0 && left.y >= 0 ? leftPos : center, center, ScriptTriggerType::MOVE_IN);
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

	for (auto& runner : m_scriptRunners) {
		runner.second->update(ellapsedTime);
	}
}

void ska::ScriptRefreshSystem::update(unsigned int ellapsedTime) {
	ScriptRefreshSystemBase::update(ellapsedTime);
}

void ska::ScriptRefreshSystem::addScriptRunner(std::string fileExtension, ScriptRunner& runner) {
	m_scriptRunners.emplace_back(std::move(fileExtension), &runner);
}

bool ska::ScriptRefreshSystem::startScript(ScriptSleepComponent script, const EntityId origin) {
	for (const auto& [extension, runner] : m_scriptRunners) {
		if (script.name.size() >= extension.size() && script.name.compare(script.name.size() - extension.size(), extension.size(), extension) == 0) {
			runner->start(script, origin);
			return script.triggeringType == ScriptTriggerType::AUTO;
		}
	}
	SLOG(LogLevel::Error) << "No scripting system to run the script \"" << script.name << "\" (unknown file extension)";

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
