#pragma once
#include "Core/ECS/Component.h"
namespace ska {
	class ForceComponent : public Component<ForceComponent> {
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
