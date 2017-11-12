#pragma once
#include "../../SerializableComponent.h"
#include "../../../Point.h"
#include "CollisionContact.h"

namespace ska {
	class WorldCollisionComponent : public SerializableComponent {
	public:
        WorldCollisionComponent();
		~WorldCollisionComponent() = default;

		CollisionContact contactX;
		CollisionContact contactY;
		bool xaxis;
		bool yaxis;
		std::vector<Point<int>> blockColPosX;
		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> blockColPosY;
		std::vector<Point<int>> lastBlockColPosY;
	};
}
