#pragma once
#include "Core/Data/Events/GameEventDispatcher.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Base/Patterns/SubObserver.h"

namespace ska {
	class CollisionContact;
	struct CollisionEvent;
	class CollidableComponent;
	class EntityManager;
	using EntityCollisionObserver = SubObserver<CollisionEvent>;

	class EntityCollisionResponse : public EntityCollisionObserver {
	public:
		EntityCollisionResponse(GameEventDispatcher& colSys, EntityManager& em);
		EntityCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, GameEventDispatcher& colSys, EntityManager& em);
		~EntityCollisionResponse();
		
		EntityCollisionResponse& operator=(const EntityCollisionResponse&) = delete;

		static bool handleInfiniteMasses(const CollisionComponent& col, float invMassOrigin, float invMassTarget, MovementComponent& mtarget, MovementComponent& morigin);
		virtual bool onEntityCollision(CollisionEvent&);
		
		static void correctPosition(ska::PositionComponent& origin, ska::PositionComponent& target, float invMassOrigin, float invMassTarget, const CollisionContact& cc, const float slope, const float percent);

	private:
		EntityManager& m_entityManager;
	};
}
