#pragma once
#include <unordered_set>
#include <vector>
#include "ECS/Basics/Script/ScriptAwareComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "ECS/System.h"
#include "ScriptAutoSystem.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Utils/SubObserver.h"

namespace ska {
	struct InputKeyEvent;
	class ScriptPositionedGetter;

	using ScriptRefreshSystemBase = System< RequiredComponent<PositionComponent, AnimationComponent, HitboxComponent, ScriptAwareComponent>, PossibleComponent<WorldCollisionComponent, ScriptSleepComponent>>;
	class ScriptRefreshSystem :
		public ScriptRefreshSystemBase,
		public SubObserver<InputKeyEvent>,
		public SubObserver<CollisionEvent>,
		public SubObserver<ScriptEvent> {

	public:
		ScriptRefreshSystem(EntityManager& entityManager, GameEventDispatcher& ged, ScriptAutoSystem& scriptAutoSystem, ScriptPositionedGetter& spg);
		~ScriptRefreshSystem() override = default;
		void update(unsigned int ellapsedTime) override;

	protected:
		void refresh(unsigned int ellapsedTime) override;
	
	private:
		bool onKeyEvent(InputKeyEvent & ike);
		bool onCollisionEvent(CollisionEvent& ce);
		void triggerChangeBlockScripts(const ScriptEvent& se);
		bool onScriptEvent(ScriptEvent& se);
		void createScriptFromSleeping(std::vector<ScriptSleepComponent> sleepings, const EntityId& parent);
		EntityId findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script, unsigned int distance = 48) const;
		bool startScript(ScriptSleepComponent script, EntityId origin);

		ScriptPositionedGetter& m_scriptPositionedGetter;
		ScriptAutoSystem& m_scriptAutoSystem;
		bool m_action;
		ska::EntityManager& m_entityManager;
	};

}
