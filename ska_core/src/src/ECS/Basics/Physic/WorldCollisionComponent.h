#pragma once
#include "../../../Point.h"

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
