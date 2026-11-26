#pragma once
#include <vector>
#include "AnimatedTexture.h"
#include "Core/ECS/Component.h"
#include "Core/Draw/FrameClock.h"
#include "Core/Draw/Renderer.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/Point.h"

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
		// Moves the drawing (pixels) from where its hitbox places it (ex : a character centred on its block)
		Point<int> drawOffset{};

		// The top left corner of the drawing in the world, its height (z) left out : placed by the hitbox, moved by
		// drawOffset. The GraphicSystem draws it there ; what follows a drawing (its shadow, a mark over it) uses it too
		Point<int> spriteOrigin(long x, long y, const HitboxComponent& hitbox) const;

		// The position to draw : between the last two logic positions of the entity (FrameClock), a jump (teleport) excepted
		Interpolated::Value drawnPosition(const PositionComponent& position);

	private:
		Interpolated m_drawn;
	};
}

SKA_DECLARE_COMPONENT(ska::GraphicComponent);
