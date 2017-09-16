#pragma once
#include "../../SerializableComponent.h"
#include "../../ECSDefines.h"
#include "../../../Point.h"
namespace ska {
	class WorldCollisionComponent : public SerializableComponent {
	public:
        WorldCollisionComponent();
		~WorldCollisionComponent() = default;

		bool xaxis;
		bool yaxis;
		std::vector<Point<int>> blockColPosX;
		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> blockColPosY;
		std::vector<Point<int>> lastBlockColPosY;
	};
}
