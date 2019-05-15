#pragma once
#include "Core/ApiDef.h"
#include "Core/ECS/Component.h"
namespace ska {
	class GravityAffectedComponent {
	public:
	    GravityAffectedComponent();
	    ~GravityAffectedComponent() = default;
		float friction;
		float rotationFriction;
	};
}

SKA_DECLARE_COMPONENT(ska::GravityAffectedComponent);