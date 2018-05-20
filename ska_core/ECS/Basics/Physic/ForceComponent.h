#pragma once

namespace ska {
	class ForceComponent {
	public:
		ForceComponent();
		~ForceComponent() = default;

		float x;
		float y;
		float z;

		float weight;
		float bounciness;
		float maxSpeed;
	};
}
