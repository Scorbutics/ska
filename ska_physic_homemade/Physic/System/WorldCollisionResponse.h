#pragma once

#include "Data/Events/GameEventDispatcher.h"
#include "Utils/SubObserver.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	class BlockAllowance;
	using WorldCollisionObserver = SubObserver<CollisionEvent>;
	class EntityManager;
	class CollisionSystem;

	class WorldCollisionResponse :
		public WorldCollisionObserver {
	public:
		WorldCollisionResponse(BlockAllowance& cp, GameEventDispatcher& ged, EntityManager& em);
		WorldCollisionResponse(const std::function<bool(CollisionEvent&)>& onEntityCollision, BlockAllowance& cp, GameEventDispatcher& colSys, EntityManager& em);
		WorldCollisionResponse& operator=(const WorldCollisionResponse&) = delete;
		virtual bool onWorldCollision(CollisionEvent& e);
		~WorldCollisionResponse();
	protected:
		EntityManager& m_entityManager;
		BlockAllowance& m_collisionProfile;
	};
}
