#pragma once
#include <unordered_set>
#include <vector>
#include <optional>
#include "Core/ECS/Basics/Script/ScriptAwareComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Core/ECS/System.h"
#include "ScriptAutoSystem.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Base/Patterns/SubObserver.h"

namespace ska {
	struct InputKeyEvent;
	class ScriptPositionedGetter;

	using ScriptRefreshSystemBase = System<RequiredComponent<PositionComponent, AnimationComponent, HitboxComponent, ScriptAwareComponent>, PossibleComponent<WorldCollisionComponent, ScriptSleepComponent>>;
	using ScriptPositionSystemBase = System<RequiredComponent<PositionComponent, ScriptSleepComponent>, PossibleComponent<>>;

	class ScriptRefreshSystem;

	class ScriptPositionSystem :
		public ScriptPositionSystemBase {
		friend class ScriptRefreshSystem;
	public:
		using ScriptPositionSystemBase::ScriptPositionSystemBase;
		~ScriptPositionSystem() override = default;

	protected:
		void refresh(unsigned int ellapsedTime) override {}
	};
	
	
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
		std::optional<EntityId> findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script, unsigned int distance = 48) const;
		bool startScript(ScriptSleepComponent script, EntityId origin);

		ScriptPositionSystem m_scriptPositionSystem;
		ScriptPositionedGetter& m_scriptPositionedGetter;
		ScriptAutoSystem& m_scriptAutoSystem;
		bool m_action;
		ska::EntityManager& m_entityManager;
	};

}
