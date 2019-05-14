#pragma once
#include <vector>
#include "AnimatedTexture.h"
#include "Core/ECS/Component.h"

namespace ska {
	class GraphicComponent : public Component<GraphicComponent> {
	public:
		GraphicComponent();
		GraphicComponent& operator=(const GraphicComponent& gc);

		int desiredPriority;
		std::vector<AnimatedTexture> animatedSprites;
		std::vector<Texture> sprites;
	};
}
