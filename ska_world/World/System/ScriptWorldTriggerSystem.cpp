#include "ScriptWorldTriggerSystem.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "../TileWorld.h"

ska::ScriptWorldTriggerSystem::ScriptWorldTriggerSystem(EntityManager& em, GameEventDispatcher& ged, TileWorld& world) :
	System(em),
	m_eventDispatcher(ged),
	m_world(world) {
}

void ska::ScriptWorldTriggerSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	
	for(auto& entityId : processed) {
		const auto& pc = m_componentAccessor.get<PositionComponent>(entityId);
		auto& sac = m_componentAccessor.get<ScriptAwareComponent>(entityId);
		const auto& hc = m_componentAccessor.get<HitboxComponent>(entityId);

		const Point<int>& centerPos = PositionComponent::getCenterPosition(pc, hc);
		const auto blockSize = m_world.getBlockSize();
		const auto oldPos = Point<int>(sac.lastBlockPos);
	
		//Blocks positioned events
		const auto lastBlock = m_world.getCollisionProfile().getHighestNonCollidingBlock(pc.layer, oldPos.x / blockSize, oldPos.y / blockSize);
		const auto newBlock = m_world.getCollisionProfile().getHighestNonCollidingBlock(pc.layer, centerPos.x / blockSize, centerPos.y / blockSize);
		const auto sameBlock = newBlock == lastBlock || (newBlock != nullptr && lastBlock != nullptr && newBlock->id != lastBlock->id);

		if (!sameBlock) {
			/* If we are moving to another block, triggers a MOVE_OUT event on previous block and MOVE_IN on the next one */
			auto scriptEvent = ScriptEvent{ ScriptEventType::EntityChangeBlockId, entityId, 0, lastBlock == nullptr ? -1 : lastBlock->id, newBlock == nullptr ? -1 : newBlock->id, oldPos, centerPos };
			m_eventDispatcher.Observable<ScriptEvent>::notifyObservers(scriptEvent);
		}

		//Auto events
		auto worldScripts = std::vector<ScriptSleepComponent*>{};
		auto autosScript = m_world.getScriptsAuto();
		for (auto& autoScript : autosScript) {
			worldScripts.push_back(&autoScript.get());
		}
		createScriptFromSleeping(worldScripts, entityId);

		//Update Script Aware Component
		if (oldPos / blockSize != centerPos / blockSize) {
			sac.lastBlockPos.x = centerPos.x;
			sac.lastBlockPos.y = centerPos.y;
		}
	}
}

void ska::ScriptWorldTriggerSystem::createScriptFromSleeping(const std::vector<ScriptSleepComponent*>& sleepings, const EntityId& parent) {
	auto& components = m_componentAccessor;

	const auto& pc = components.get<PositionComponent>(parent);
	
	for (const auto& ssc : sleepings) {
		if (ssc != nullptr) {
			const auto scriptCreated = createEntity();
			
			components.add<PositionComponent>(scriptCreated, PositionComponent(pc));
			auto sscAdd = ScriptSleepComponent(*ssc);
			sscAdd.deleteEntityWhenFinished = true;
			components.add<ScriptSleepComponent>(scriptCreated, std::move(sscAdd));

			auto se = ScriptEvent{ ScriptEventType::ScriptCreate, scriptCreated, parent, {}, {}, {}, pc };
			m_eventDispatcher.Observable<ScriptEvent>::notifyObservers(se);
		}
	}

	
}

