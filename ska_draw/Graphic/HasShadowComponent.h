#pragma once
#include "ECS/SerializableComponent.h"

namespace ska {
	class HasShadowComponent : public SerializableComponent {
	public:
	    HasShadowComponent();
	    ~HasShadowComponent() = default;
	};
}
