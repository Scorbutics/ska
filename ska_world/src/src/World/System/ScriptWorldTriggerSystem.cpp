#include "ScriptWorldTriggerSystem.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
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
		const auto lastBlock = m_world.getCollisionProfile().getBlock(pc.layer, oldPos.x / blockSize, oldPos.y / blockSize);
		const auto newBlock = m_world.getCollisionProfile().getBlock(pc.layer, centerPos.x / blockSize, centerPos.y / blockSize);
		auto events = std::vector<ScriptEventType>{};

		const auto sameBlockId = newBlock == lastBlock || (newBlock != nullptr && lastBlock != nullptr && newBlock->id == lastBlock->id);	
		if (!sameBlockId) {
			/* If we are moving to another block, triggers a MOVE_OUT event on previous block and MOVE_IN on the next one */
			events.push_back(ScriptEventType::EntityChangeBlockId);
		}

		/*const auto sameBlockProperty = newBlock == lastBlock || (newBlock != nullptr && lastBlock != nullptr && newBlock->properties.bitMask == lastBlock->properties.bitMask);
		if (!sameBlockProperty) {
			events.push_back(ScriptEventType::EntityChangeBlockProperty);
		}*/

		auto scriptEvent = ScriptEvent{ std::move(events), {}, 0, lastBlock == nullptr ? -1 : lastBlock->id, newBlock == nullptr ? -1 : newBlock->id, oldPos, centerPos };
		m_eventDispatcher.Observable<ScriptEvent>::notifyObservers(scriptEvent);

		//Auto events
		auto worldScripts = std::vector<ScriptSleepComponent>{};
		const auto autosScript = m_world.getScriptsAuto();
		for (const auto& autoScript : autosScript) {
			worldScripts.push_back(autoScript.get().scripts);
		}
		createScriptFromSleeping(std::move(worldScripts), entityId);

		//Update Script Aware Component
		if (oldPos / blockSize != centerPos / blockSize) {
			sac.lastBlockPos.x = (centerPos.x / blockSize) * blockSize;
			sac.lastBlockPos.y = (centerPos.y / blockSize) * blockSize;
		}
	}
}

void ska::ScriptWorldTriggerSystem::createScriptFromSleeping(std::vector<ScriptSleepComponent> sleepings, const EntityId& parent) {
	auto& components = m_componentAccessor;

	const auto& pc = components.get<PositionComponent>(parent);
	
	for (auto& ssc : sleepings) {
		//const auto scriptCreated = createEntity();
			
		//components.add<PositionComponent>(scriptCreated, PositionComponent(pc));
		auto sscAdd = std::move(ssc);
		sscAdd.deleteEntityWhenFinished = true;
		//components.add<ScriptSleepComponent>(scriptCreated, std::move(sscAdd));

		auto events = std::vector<ScriptEventType>();
		events.push_back(ScriptEventType::ScriptCreate);
		auto se = ScriptEvent{ std::move(events), std::move(ssc), parent, {}, {}, {}, pc };
		m_eventDispatcher.Observable<ScriptEvent>::notifyObservers(se);	
	}

	
}

