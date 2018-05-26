#pragma once
#include <cstddef>

namespace ska {
	namespace cp {
		struct HitboxComponent {
			static std::size_t bodies;
			std::size_t bodyIndex{};
			std::size_t shapeIndex{};
		};
	}
}
