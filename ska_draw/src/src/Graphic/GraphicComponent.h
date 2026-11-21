#pragma once
#include <vector>
#include "AnimatedTexture.h"
#include "Core/ECS/Component.h"
#include "Core/Draw/FrameClock.h"
#include "Core/Draw/Renderer.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"

namespace ska {
	class GraphicComponent {
	public:
		GraphicComponent();
		GraphicComponent& operator=(const GraphicComponent& gc);

		int desiredPriority;
		std::vector<AnimatedTexture> animatedSprites;
		std::vector<Texture> sprites;
		// Changes of the drawing of the entity (ex : a white flash when it is hit)
		RenderEffect effect;

		// The position to draw : between the last two logic positions of the entity (FrameClock), a jump (teleport) excepted
		Interpolated::Value drawnPosition(const PositionComponent& position);

	private:
		Interpolated m_drawn;
	};
}

SKA_DECLARE_COMPONENT(ska::GraphicComponent);
