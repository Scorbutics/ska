#pragma once
#include "../../SerializableComponent.h"
namespace ska {
	struct MovementComponent : public SerializableComponent {
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
