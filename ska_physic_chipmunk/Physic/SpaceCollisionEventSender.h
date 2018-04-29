#pragma once
#include "Space.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	namespace cp {
		class SpaceCollisionEventSender {
		public:
			SpaceCollisionEventSender(Space& space, GameEventDispatcher& ged, unsigned int blockSize);
			~SpaceCollisionEventSender();

		private:
			Space& m_space;
			std::size_t m_callbackIndex;
		};
	}
}
