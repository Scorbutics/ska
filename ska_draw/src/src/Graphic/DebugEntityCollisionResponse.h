#pragma once
#include "Core/Data/Events/GameEventDispatcher.h"
#include "Base/Patterns/SubObserver.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class EntityManager;
	using EntityCollisionObserver = SubObserver<CollisionEvent>;

	class DebugEntityCollisionResponse :
		public EntityCollisionObserver {
	public:
		DebugEntityCollisionResponse(GameEventDispatcher& ged, EntityManager& em);
		DebugEntityCollisionResponse& operator=(const DebugEntityCollisionResponse&) = delete;
		bool onEntityCollision(CollisionEvent& e) const;
	private:
		EntityManager& m_entityManager;
		GameEventDispatcher& m_ged;
	};
}
