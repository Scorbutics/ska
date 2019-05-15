#pragma once
#include <cstddef>
#include "Core/ECS/Component.h"
namespace ska {
	struct HitboxComponent {
	    HitboxComponent();
	    ~HitboxComponent() = default;
		int xOffset;
		int yOffset;
		unsigned int width;
		unsigned int height;
	};

	namespace cp {
		struct HitboxComponent {
			std::size_t bodyIndex{};
			std::size_t controlBodyIndex{};
			std::size_t shapeIndex{};
		};
	}
}

SKA_DECLARE_COMPONENT(ska::HitboxComponent);
SKA_DECLARE_COMPONENT(ska::cp::HitboxComponent);