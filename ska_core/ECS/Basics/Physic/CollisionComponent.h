#pragma once
#include "../../Component.h"
#include "../../ECSDefines.h"
#include "../../../Point.h"

namespace ska {
	class CollisionComponent : public Component {
	public:
		CollisionComponent();
		EntityId origin;
		EntityId target;
		bool xaxis;
		bool yaxis;
		Rectangle overlap;

		float penetration;
		Point<float> normal;
		
	};
}
