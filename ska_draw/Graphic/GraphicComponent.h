#pragma once
#include <vector>
#include "AnimatedTexture.h"

namespace ska {
	class GraphicComponent {
	public:
		GraphicComponent();
		GraphicComponent& operator=(const GraphicComponent& gc);

		int desiredPriority;
		std::vector<AnimatedTexture> animatedSprites;
		std::vector<Texture> sprites;
	};
}
