#pragma once

#include "Data/Events/GameEventDispatcher.h"
#include "Utils/SubObserver.h"

namespace ska {
	class TileWorld;
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	using WorldCollisionObserver = SubObserver<CollisionEvent>;
	class EntityManager;
	class CollisionSystem;

	class WorldCollisionResponse :
		public WorldCollisionObserver {
	public:
		WorldCollisionResponse(TileWorld& cp, GameEventDispatcher& ged, EntityManager& em);
		WorldCollisionResponse(const std::function<bool(CollisionEvent&)>& onEntityCollision, TileWorld& cp, GameEventDispatcher& colSys, EntityManager& em);
		WorldCollisionResponse& operator=(const WorldCollisionResponse&) = delete;
		virtual bool onWorldCollision(CollisionEvent& e);
		~WorldCollisionResponse();
	protected:
		EntityManager& m_entityManager;
		TileWorld& m_world;
	};
}
