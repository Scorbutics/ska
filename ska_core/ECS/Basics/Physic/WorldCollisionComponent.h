#pragma once
#include "CollisionContact.h"

namespace ska {
	class WorldCollisionComponent {
	public:
        WorldCollisionComponent();
		~WorldCollisionComponent() = default;

		CollisionContact contactX;
		CollisionContact contactY;
		bool xaxis;
		bool yaxis;

		using BlockCollisionContainer = std::vector<Rectangle>;

		BlockCollisionContainer blockColPosX;
		BlockCollisionContainer blockColPosY;
		
		/*BlockCollisionContainer lastBlockColPosX;
		BlockCollisionContainer lastBlockColPosY;*/
	};
}
