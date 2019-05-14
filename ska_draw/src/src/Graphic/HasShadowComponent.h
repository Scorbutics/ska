#pragma once
#include "Core/ECS/Component.h"

namespace ska {
	class HasShadowComponent : public Component<HasShadowComponent> {
	public:
	    HasShadowComponent();
	    ~HasShadowComponent() = default;
	};
}
