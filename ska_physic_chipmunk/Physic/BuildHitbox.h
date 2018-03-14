#pragma once
#include "HitboxComponent.h"
#include "Point.h"

namespace ska {
	namespace cp {
		class Space;
		ska::cp::HitboxComponent BuildHitbox(ska::cp::Space& space, const Point<float>& point, float friction, float rotateFriction);
	}
}
