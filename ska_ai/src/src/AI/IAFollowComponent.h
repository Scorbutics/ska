#pragma once
#include "Core/ECS/ECSDefines.h"
#include "Core/ECS/Component.h"

namespace ska {
	// The entity walks after another one (IAFollowSystem), staying at "distance" (pixels) from it
	class IAFollowComponent {
	public:
		IAFollowComponent() = default;
		IAFollowComponent(EntityId target, float distance) : target(target), distance(distance) {}

		EntityId target = 0;
		float distance = 48.F;
	};
}

SKA_DECLARE_COMPONENT(ska::IAFollowComponent);
