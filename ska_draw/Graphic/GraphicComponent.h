#pragma once
#include <vector>
#include "AnimatedTexture.h"
#include "ECS/Component.h"

namespace ska {
	class GraphicComponent : 
		public Component {
	public:
		GraphicComponent();
		void operator=(const GraphicComponent& gc);

		int desiredPriority;
		std::vector<AnimatedTexture> animatedSprites;
		std::vector<Texture> sprites;
	};
}
