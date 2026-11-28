#pragma once

#include "Core/Draw/DrawableFixedPriority.h"
#include "Core/Point.h"
#include "Base/Containers/Vector2.h"
#include "Graphic/Texture.h"
#include "LayerRenderable.h"

namespace ska {
	class Texture;
	class TileWorld;
	struct TileAnimation;

	class LayerMonoRenderable : 
		public LayerRenderable,
		public MovableNonCopyable,
		public DrawableFixedPriority {
	public:
		// origin : where the layer is in the world (pixels)
		LayerMonoRenderable(Vector2<TileAnimation*> block, const Texture& tileset, unsigned int blockSize, Point<int> origin = {});
		~LayerMonoRenderable() = default;
	
		void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) override;

	private:
		void render(Renderer& renderer) const override;

		const unsigned int m_tileSize;
		const Point<int> m_origin;

		Vector2<TileAnimation*> m_animations;
		Texture m_tileset;

		Rectangle m_lastCameraPos {};
		unsigned int m_width {};
		unsigned int m_height {};

	};

}

