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
    class BlockAllowance;

	using ScriptPositionSystemAccess = System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, ScriptSleepComponent>, PossibleComponent<>>;
	using ScriptRefreshSystemBase = System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, AnimationComponent, HitboxComponent, ScriptAwareComponent>, PossibleComponent<WorldCollisionComponent>>;
	class ScriptRefreshSystem :
		public ScriptRefreshSystemBase,
		/* Allows easy access to each entity that contains ScriptSleepComponent and PositionComponent */
		public ScriptPositionSystemAccess,
		public SubObserver<InputKeyEvent>,
		public SubObserver<CollisionEvent>,
		public SubObserver<ScriptEvent> {

	public:
		ScriptRefreshSystem(EntityManager& entityManager, GameEventDispatcher& ged, ScriptAutoSystem& scriptAutoSystem, ScriptPositionedGetter& spg, BlockAllowance& world);
		void registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity);
		void clearNamedScriptedEntities();
		~ScriptRefreshSystem() override = default;
		void update(unsigned int ellapsedTime) override;

	protected:
		void refresh(unsigned int ellapsedTime) override;
	
	private:
		void createScriptFromSleeping(const std::vector<ScriptSleepComponent*>& sleepings, const EntityId& parent);
		std::vector<ScriptSleepComponent*> refreshScripts(const ska::EntityId& entityId);
		bool onKeyEvent(InputKeyEvent & ike);
		bool onCollisionEvent(CollisionEvent& ce);
		bool onScriptEvent(ScriptEvent& se);
		EntityId findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script) const;
		void startScript(const EntityId scriptEntity, const EntityId origin);

		ScriptPositionedGetter& m_scriptPositionedGetter;
		BlockAllowance& m_world;
		ScriptAutoSystem& m_scriptAutoSystem;
		bool m_action;
		GameEventDispatcher& m_eventDispatcher;
	};

}
