#pragma once
#include <cstddef>
#include "Core/ECS/ComponentTag.h"
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
#ifndef SKA_CORE_IN
		extern template class SKA_CORE_API ComponentTag<HitboxComponent>;
#endif
	}

#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<HitboxComponent>;
#endif
}