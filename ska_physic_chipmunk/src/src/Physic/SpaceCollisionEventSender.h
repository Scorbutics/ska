#pragma once
#include "Space.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	class EntityManager;

	namespace cp {
		class SpaceCollisionEventSender {
		public:
			SpaceCollisionEventSender(EntityManager& em, Space& space, GameEventDispatcher& ged, unsigned int blockSize);
			WorldCollisionComponent createWorldCollisionFromArbiter(ska::cp::Arbiter& arb, ska::EntityId* entityId, unsigned int blockSize);
			~SpaceCollisionEventSender();

		private:
			Space& m_space;
			EntityManager& m_entityManager;
			std::size_t m_callbackIndex;
		};
	}
}
