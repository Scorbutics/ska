#pragma once
#include "LayerRenderable.h"
#include "Graphic/Texture.h"
#include "TileAnimation.h"
#include "Base/Containers/Vector2.h"
#include "LayerRenderableRow.h"

namespace ska {
	struct Rectangle;
	class DrawableContainer;

	class LayerYIndexRenderable :
		public LayerRenderable {
	public:
		LayerYIndexRenderable(Vector2<TileAnimation*> block, const Texture& tileset, unsigned int blockSize, unsigned int priority);
		void graphicUpdate(const Rectangle& cameraPos, DrawableContainer& drawables) override;
		~LayerYIndexRenderable() override = default;

		void setPriority(unsigned int priority);

	private:
		const unsigned int m_blockSize;

		std::vector<LayerRenderableRow> m_lines;
		Texture m_tileset;
	};	
}
