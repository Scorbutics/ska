#pragma once
#include "HitboxComponent.h"
#include "Point.h"
#include "ECS/ECSDefines.h"

struct cpBody;

namespace ska {
	namespace cp {
		class Space;
		class Body;
		
		HitboxComponent BuildControlledRectangleHitbox(Space& space, const ska::Rectangle& box, double mass, EntityId entityId);
		HitboxComponent BuildRectangleHitbox(Space& space, const ska::Rectangle& box, double mass, EntityId entityId);
		void AddTopDownConstraints(Space& space, Body* bodyToBeLinked, Body& bodyCreated, const float friction, const float rotateFriction);
	}
}
