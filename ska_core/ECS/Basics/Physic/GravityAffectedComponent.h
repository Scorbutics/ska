#pragma once

namespace ska {
	class GravityAffectedComponent {
	public:
	    GravityAffectedComponent();
	    ~GravityAffectedComponent() = default;
		float friction;
		float rotationFriction;
	};
}
