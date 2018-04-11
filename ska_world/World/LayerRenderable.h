#pragma once

#include <memory>

#include "BlockRenderable.h"
#include "Draw/DrawableFixedPriority.h"
#include "Utils/Vector2.h"
#include "TilesetRenderable.h"

namespace ska {
	class TileWorld;
	class LayerRenderable : public DrawableFixedPriority {
	public:
		LayerRenderable(Vector2<BlockRenderable*>&& block, TilesetRenderable& chipset, unsigned int blockSize);
		void operator=(const LayerRenderable&) = delete;
		~LayerRenderable() = default;

		void update(const ska::Rectangle& cameraPos);
		void clear();

		void render(const Renderer& renderer) const override;
		bool isVisible() const override;

		BlockRenderable* getBlock(unsigned int i, unsigned int j);

	private:

		const unsigned int m_blockSize;
		Vector2<BlockRenderable*> m_block;
		TilesetRenderable& m_tileset;
		ska::Rectangle m_lastCameraPos;
		unsigned int m_width;
		unsigned int m_height;

	};
	typedef std::unique_ptr<LayerRenderable> LayerRenderablePtr;


}

