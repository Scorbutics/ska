#pragma once

namespace ska {
	namespace cp {
		struct HitboxComponent {
			std::size_t bodyIndex{};
			std::size_t shapeIndex{};
		};
	}
}
