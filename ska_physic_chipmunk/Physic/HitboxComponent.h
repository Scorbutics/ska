#pragma once
#include <cstddef>

namespace ska {
	namespace cp {
		struct HitboxComponent {
			std::size_t bodyIndex{};
			std::size_t controlBodyIndex{};
			std::size_t shapeIndex{};
		};
	}
}
