#pragma once
#include "../../SerializableComponent.h"
#include "../../ECSDefines.h"
#include "../../../Point.h"

namespace ska {
	class CollisionComponent : public SerializableComponent {
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
