#pragma once
#include "Core/Data/Events/GameEventDispatcher.h"
#include "Base/Patterns/SubObserver.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	using WorldCollisionObserver = SubObserver<CollisionEvent>;
	class EntityManager;

	class DebugWorldCollisionResponse :
		public WorldCollisionObserver {
	public:
		DebugWorldCollisionResponse(GameEventDispatcher& ged, EntityManager& em);
		DebugWorldCollisionResponse& operator=(const DebugWorldCollisionResponse&) = delete;
		bool onWorldCollision(CollisionEvent& e) const;
		~DebugWorldCollisionResponse();
	private:
		EntityManager& m_entityManager;
	};
}
