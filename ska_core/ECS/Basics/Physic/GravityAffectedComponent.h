#pragma once
#include "../../SerializableComponent.h"
namespace ska {
	class GravityAffectedComponent : public SerializableComponent {
	public:
	    GravityAffectedComponent();
	    ~GravityAffectedComponent() = default;
		float friction;
	};
}
