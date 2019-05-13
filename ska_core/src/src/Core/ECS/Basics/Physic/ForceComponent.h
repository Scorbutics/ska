#pragma once
#include "Core/ECS/ComponentTag.h"
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
#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<ForceComponent>;
#endif
}
