#pragma once
#include <unordered_set>
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Data/Events/CollisionEvent.h"
#include "Rectangle.h"
#include "ECS/System.h"
#include "Data/Events/GameEventDispatcher.h"


namespace ska {
	class WorldCollisionComponent;
	class CollisionProfile;

	class WorldCollisionSystem : public System< RequiredComponent<PositionComponent, MovementComponent, HitboxComponent, CollidableComponent>, PossibleComponent<WorldCollisionComponent>> {
	public:
		WorldCollisionSystem(EntityManager& entityManager, const CollisionProfile& w, GameEventDispatcher& ged);
		WorldCollisionSystem& operator=(const WorldCollisionSystem&) = delete;
		~WorldCollisionSystem() override = default;
	protected:
		Rectangle calculateOverlap(Rectangle nextPos, const std::vector<Rectangle>& points);
		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		inline Rectangle createHitBox(EntityId entityId, bool xaxis, bool noMove) const;
		const CollisionProfile& m_collisionProfile;
		GameEventDispatcher& m_ged;
	};
}
