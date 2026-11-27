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
		// Sorted with the drawings of the map as if its position were on a grid of this step (pixels, 0 : its position) :
		// a character walking block by block keeps the depth of the upper of its two blocks (walking down : the block it
		// leaves until it arrives ; walking up : the block it goes to, at once), as the objects of the map in front of it
		// (ex : the leaves of the tall grass) cover it during all of its step
		int sortStep = 0;
		// Pixels of its bottom not drawn (ex : its legs in tall grass), over its effect (which the game may reset at each
		// frame : a fighter)
		int cropBottom = 0;
		// Hidden behind something (the game decides) : drawn again over the world (GraphicSystem::setSilhouettePriority), dark,
		// with this opacity (0 : no silhouette)
		unsigned char silhouette = 0;

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
