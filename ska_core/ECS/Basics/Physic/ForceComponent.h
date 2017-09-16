#pragma once
#include "../../SerializableComponent.h"

namespace ska {
	class ForceComponent : public SerializableComponent {
	public:
		ForceComponent();
		~ForceComponent() = default;

		float x;
		float y;
		float z;

		float weight;
		float bounciness;
	};
}
