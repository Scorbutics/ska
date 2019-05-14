#pragma once
#include <cstddef>
#include "Core/ECS/Component.h"
namespace ska {
	struct HitboxComponent : public Component<HitboxComponent> {
	    HitboxComponent();
	    ~HitboxComponent() = default;
		int xOffset;
		int yOffset;
		unsigned int width;
		unsigned int height;
	};

	namespace cp {
		struct HitboxComponent : public Component<HitboxComponent> {
			std::size_t bodyIndex{};
			std::size_t controlBodyIndex{};
			std::size_t shapeIndex{};
		};
	}
}
