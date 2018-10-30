#pragma once

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
