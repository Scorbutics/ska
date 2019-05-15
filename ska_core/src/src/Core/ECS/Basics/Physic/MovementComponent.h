#pragma once
#include "Core/ApiDef.h"
#include "Core/ECS/Component.h"
namespace ska {
	struct MovementComponent {
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
SKA_DECLARE_COMPONENT(ska::MovementComponent);