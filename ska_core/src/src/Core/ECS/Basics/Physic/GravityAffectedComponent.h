#pragma once
#include "Core/ECS/Component.h"
namespace ska {
	class GravityAffectedComponent : public Component<GravityAffectedComponent> {
	public:
	    GravityAffectedComponent();
	    ~GravityAffectedComponent() = default;
		float friction;
		float rotationFriction;
	};
}
