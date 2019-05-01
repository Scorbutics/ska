#pragma once
#include <cstddef>

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