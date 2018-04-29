#pragma once
#include <queue>
#include "../../ECSDefines.h"
#include "../Physic/WorldCollisionComponent.h"

namespace ska {
	enum DebugGraphicType {
		COLLISION = (1u << 0),
		WALK = (1u << 1)
	};
	class DebugGraphicComponent {
	public:
		DebugGraphicComponent();
		virtual ~DebugGraphicComponent() = default;
		
		unsigned int typeMask;
		std::queue<EntityId> collidedBlocks;
		std::vector<Rectangle> blockColPosX;
		std::vector<Rectangle> blockColPosY;
	};
}
