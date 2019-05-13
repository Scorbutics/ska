#pragma once
#include "Core/ECS/ComponentTag.h"
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
#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<MovementComponent>;
#endif
}
