#pragma once
#include <queue>
#include "../../ECSDefines.h"
#include "../Physic/WorldCollisionComponent.h"
#include "Core/ECS/ComponentTag.h"
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

#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<DebugGraphicComponent>;
#endif
}
