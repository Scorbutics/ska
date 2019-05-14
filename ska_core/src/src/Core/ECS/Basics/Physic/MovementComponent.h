#pragma once
#include "Core/ECS/Component.h"
namespace ska {
	struct MovementComponent : public Component<MovementComponent> {
		MovementComponent();
        ~MovementComponent() = default;

		float ax;
		float ay;
		float az;

		float vx;
		float vy;
		float vz;
	};
}
