#pragma once
#include "Graphic/Texture.h"
#include "Graphic/Animation.h"
#include "../World/BlockRenderable.h"

namespace ska {

	class TilesetRenderable {
	public:
		TilesetRenderable(const unsigned int corrFileSize, const int blockSize, const std::string& chipsetName);
		void render(const Renderer& renderer, Point<int> pos, const BlockRenderable& block) const;
		void update(BlockRenderable& block);
		
		BlockRenderable& getBlock(const int id, const int blockSize, Point<int> posCorr, bool auto_anim) const;

		const Texture& getTexture() const;
		~TilesetRenderable() = default;

	private:
		mutable std::vector<BlockRenderablePtr> m_blocks;
		void load(const std::string& chipsetName);

		Texture m_chipset;
		Animation m_animBlocks;
	};
}
