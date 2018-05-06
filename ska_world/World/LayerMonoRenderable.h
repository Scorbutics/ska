#pragma once

#include "Draw/DrawableFixedPriority.h"
#include "Utils/Vector2.h"
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
		LayerMonoRenderable(Vector2<TileAnimation*> block, const Texture& tileset, unsigned int blockSize);
		~LayerMonoRenderable() = default;
	
		void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) override;

	private:
		void render(const Renderer& renderer) const override;

		const unsigned int m_tileSize;

		Vector2<TileAnimation*> m_animations;
		Texture m_tileset;

		Rectangle m_lastCameraPos {};
		unsigned int m_width {};
		unsigned int m_height {};

	};

}

