#pragma once
#include "../../../Point.h"
#include "Core/ECS/Component.h"

namespace ska {
	class WorldCollisionComponent {
	public:
        WorldCollisionComponent() = default;
		~WorldCollisionComponent() = default;

		std::vector<Point<int>> blockContacts;
		Point<int> lastBlockDirection;
		Point<float> normal;
		// Sent by the physics (a contact), not by a movement system (ex : a character pushing against a wall on purpose)
		bool fromPhysics = false;
	};
}
SKA_DECLARE_COMPONENT(ska::WorldCollisionComponent);