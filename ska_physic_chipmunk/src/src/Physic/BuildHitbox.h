#pragma once
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "Point.h"
#include "ECS/ECSDefines.h"

struct cpBody;

namespace ska {
	namespace cp {
		class Space;
		class Body;
		
		HitboxComponent BuildControlledRectangleHitbox(Space& space, const ska::Point<float>& pos, const ska::Rectangle& box, double mass, EntityId entityId);
		HitboxComponent BuildRectangleHitbox(Space& space, const ska::Point<float>& pos, const ska::Rectangle& box, double mass, EntityId entityId);
		void AddTopDownConstraints(Space& space, HitboxComponent& hc, const float friction, const float rotateFriction);
	}
}
