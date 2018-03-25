#pragma once
#include "HitboxComponent.h"
#include "Point.h"

namespace ska {
	namespace cp {
		class Space;
		ska::cp::HitboxComponent BuildCircleHitbox(ska::cp::Space& space, const Point<float>& point, float friction, float rotateFriction);
		ska::cp::HitboxComponent BuildRectangleHitbox(ska::cp::Space& space, const ska::Rectangle& box, float friction, float rotateFriction);
	}
}
