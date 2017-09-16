#pragma once
#include "../../SerializableComponent.h"

namespace ska {
	struct HitboxComponent : public SerializableComponent {
	public:
	    HitboxComponent();
	    ~HitboxComponent() = default;
		int xOffset;
		int yOffset;
		unsigned int width;
		unsigned int height;
	};
}
