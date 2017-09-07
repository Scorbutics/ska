#pragma once
#include "Utils/Observer.h"
#include "Data/Events/GameEventDispatcher.h"
#include "ECS/Basics/Physic/PositionComponent.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class EntityManager;
	using EntityCollisionObserver = Observer<CollisionEvent>;

	class EntityCollisionResponse : public EntityCollisionObserver {
	public:
		EntityCollisionResponse(GameEventDispatcher& colSys, EntityManager& em);
		EntityCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, GameEventDispatcher& colSys, EntityManager& em);
		~EntityCollisionResponse();
		
		EntityCollisionResponse& operator=(const EntityCollisionResponse&) = delete;

		virtual bool onEntityCollision(CollisionEvent&);
	private:
		bool calculateNormalAndPenetration(ska::CollisionComponent& col) const;
		static void correctPosition(ska::PositionComponent& origin, ska::PositionComponent& target, float invMassOrigin, float invMassTarget, float penetration, ska::Point<float>& n);
		GameEventDispatcher& m_ged;
		EntityManager& m_entityManager;
	};
}
