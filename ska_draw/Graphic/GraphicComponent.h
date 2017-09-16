#pragma once
#include <vector>
#include "AnimatedTexture.h"
#include "ECS/SerializableComponent.h"

namespace ska {
	class GraphicComponent : 
		public SerializableComponent {
	public:
		GraphicComponent();
		void operator=(const GraphicComponent& gc);

		int desiredPriority;
		std::vector<AnimatedTexture> animatedSprites;
		std::vector<Texture> sprites;
	};
}
