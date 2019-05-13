#pragma once
#include "Core/ECS/ComponentTag.h"
namespace ska {
	class GravityAffectedComponent {
	public:
	    GravityAffectedComponent();
	    ~GravityAffectedComponent() = default;
		float friction;
		float rotationFriction;
	};
#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<GravityAffectedComponent>;
#endif
}
