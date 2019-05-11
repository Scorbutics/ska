#pragma once
#include "Core/ECS/Basics/Script/ScriptAwareComponent.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	class TileWorld;

	class ScriptWorldTriggerSystem : 
		public System< RequiredComponent<PositionComponent, AnimationComponent, HitboxComponent, ScriptAwareComponent>, PossibleComponent<>> {
	public:
		ScriptWorldTriggerSystem(EntityManager& em, GameEventDispatcher& ged, TileWorld& world);
		~ScriptWorldTriggerSystem() override = default;

		void refresh(unsigned int ellapsedTime) override;
		
	private:
		void createScriptFromSleeping(std::vector<ScriptSleepComponent> sleepings, const EntityId& parent);

		GameEventDispatcher& m_eventDispatcher;		
		TileWorld& m_world;
	};
}
