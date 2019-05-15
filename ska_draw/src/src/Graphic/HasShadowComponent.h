#pragma once
#include "Core/ECS/Component.h"

namespace ska {
	class HasShadowComponent {
	public:
	    HasShadowComponent();
	    ~HasShadowComponent() = default;
	};
}
SKA_DECLARE_COMPONENT(ska::HasShadowComponent);