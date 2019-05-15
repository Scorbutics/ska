#pragma once
#include "../../../Point.h"
#include "Core/ApiDef.h"
#include "Core/ECS/Component.h"

namespace ska {
	class WorldCollisionComponent {
	public:
        WorldCollisionComponent() = default;
		~WorldCollisionComponent() = default;

		std::vector<Point<int>> blockContacts;
		Point<int> lastBlockDirection;
		Point<float> normal;
	};
}
SKA_DECLARE_COMPONENT(ska::WorldCollisionComponent);