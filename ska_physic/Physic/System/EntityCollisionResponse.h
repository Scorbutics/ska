#pragma once
#include "Utils/Observer.h"
#include "Data/Events/GameEventDispatcher.h"

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
		GameEventDispatcher& m_ged;
		EntityManager& m_entityManager;
	};
}
