#pragma once
#include <memory>
#include "Draw/Color.h"
#include "Graphic/SDLSurface.h"
#include "TilesetRenderable.h"
#include "TilesetCorrespondanceMapper.h"

namespace ska {
	class Animation;
	class Block;
	typedef std::unique_ptr<Block> BlockPtr;
	class BlockRenderable;
	typedef std::unique_ptr<BlockRenderable> BlockRenderablePtr;
	typedef char ScriptTriggerType;

	class Tileset {
	public:
		Tileset(TilesetCorrespondanceMapper correspondanceMapper, const int blockSize, const std::string& chipsetName);
		std::pair<ska::Block*, ska::BlockRenderable*> getBlock(const Color& key) const;
		const std::string& getName() const;
		TilesetRenderable& getRenderable();
		~Tileset() = default;

		void operator=(const Tileset&) = delete;

	private:
		void load();

		const TilesetCorrespondanceMapper m_chipsetCorrespondanceMapper;

		mutable std::vector<BlockPtr> m_blocks;

		const int m_blockSize;
		std::string m_chipsetName;
		SDLSurface m_sCol;
		SDLSurface m_sChipset;
		SDLSurface m_sProperties;

		TilesetRenderable m_renderable;

		Uint32 m_darkColor{};
		Uint32 m_whiteColor{};
		Uint32 m_lightColor{};
	};
}
