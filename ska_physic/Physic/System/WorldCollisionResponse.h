#pragma once

#include "Data/Events/GameEventDispatcher.h"
#include "Utils/SubObserver.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	class CollisionProfile;
	using WorldCollisionObserver = SubObserver<CollisionEvent>;
	class EntityManager;
	class CollisionSystem;

	class WorldCollisionResponse :
		public WorldCollisionObserver {
	public:
		WorldCollisionResponse(CollisionProfile& cp, GameEventDispatcher& ged, EntityManager& em);
		WorldCollisionResponse(const std::function<bool(CollisionEvent&)>& onEntityCollision, CollisionProfile& cp, GameEventDispatcher& colSys, EntityManager& em);
		WorldCollisionResponse& operator=(const WorldCollisionResponse&) = delete;
		virtual bool onWorldCollision(CollisionEvent& e);
		~WorldCollisionResponse();
	protected:
		EntityManager& m_entityManager;
		CollisionProfile& m_collisionProfile;
	};
}
