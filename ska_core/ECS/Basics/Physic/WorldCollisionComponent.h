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

		using BlockCollisionContainer = std::vector<Rectangle>;

		BlockCollisionContainer blockColPosX;
		BlockCollisionContainer lastBlockColPosX;
		BlockCollisionContainer blockColPosY;
		BlockCollisionContainer lastBlockColPosY;
	};
}
