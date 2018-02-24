#pragma once
#include <memory>
#include "Graphic/Color.h"
#include "Graphic/SDLSurface.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "ChipsetRenderable.h"
#include "ChipsetCorrespondanceMapper.h"

namespace ska {
	class Animation;
	class Block;
	typedef std::unique_ptr<Block> BlockPtr;
	class BlockRenderable;
	typedef std::unique_ptr<BlockRenderable> BlockRenderablePtr;
	typedef char ScriptTriggerType;

	//Rename Tileset
	class Chipset {
	public:
		Chipset(const int blockSize, const std::string& chipsetName);
		std::pair<ska::Block*, ska::BlockRenderable*> generateBlock(const Color& key);
		const std::string& getName() const;
		ChipsetRenderable& getRenderable();
		~Chipset() = default;

		void operator=(const Chipset&) = delete;

	private:
		void load();

		const ChipsetCorrespondanceMapper m_chipsetCorrespondanceMapper;

		std::vector<BlockPtr> m_blocks;

		const int m_blockSize;
		std::string m_chipsetName;
		SDLSurface m_sCol;
		SDLSurface m_sChipset;
		SDLSurface m_sProperties;

		ChipsetRenderable m_renderable;

		Uint32 m_darkColor;
		Uint32 m_whiteColor;
		Uint32 m_lightColor;
	};
}
