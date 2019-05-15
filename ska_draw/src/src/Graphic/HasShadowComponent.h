#pragma once
#include "Graphic/ApiDef.h"
#include "Core/ECS/Component.h"

namespace ska {
	class HasShadowComponent {
	public:
	    HasShadowComponent();
	    ~HasShadowComponent() = default;
	};
}
SKA_DECLARE_COMPONENT(ska::HasShadowComponent);