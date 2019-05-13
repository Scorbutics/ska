#pragma once
#include <vector>
#include "AnimatedTexture.h"
#include "Core/ECS/ComponentTag.h"

namespace ska {
	class GraphicComponent {
	public:
		GraphicComponent();
		GraphicComponent& operator=(const GraphicComponent& gc);

		int desiredPriority;
		std::vector<AnimatedTexture> animatedSprites;
		std::vector<Texture> sprites;
	};
#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<GraphicComponent>;
#endif
}
