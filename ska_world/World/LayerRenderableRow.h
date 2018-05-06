#pragma once
#include <cstddef>
#include "Draw/DrawableFixedPriority.h"
#include "Utils/Vector2.h"

namespace ska {
	struct TileAnimation;
	class Texture;
	class DrawableContainer;

	class LayerRenderableRow :
		public DrawableFixedPriority {
	public:
		LayerRenderableRow() = default;
		~LayerRenderableRow() override = default;

		void load(std::size_t rowIndex, Vector2<TileAnimation*>& blocks, const Texture& tileset, unsigned int blockSize);
		void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables);
		void render(const Renderer& renderer) const override;

	private:
		const Texture* m_tileset {};
		unsigned int m_blockSize = 1;
		unsigned int m_width = 1;
		unsigned int m_offset {};
		std::size_t m_rowIndex {};
		Rectangle m_lastCameraPos {};
		std::vector<TileAnimation*> m_animations;
	};	
	
}

	