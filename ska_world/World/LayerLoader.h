#pragma once
#include "Graphic/SDLSurface.h"
#include "ChipsetCorrespondanceMapper.h"
#include "Block.h"

namespace ska {
	class Chipset;
	class ChipsetCorrespondanceMap;

	class LayerLoader {
		using BlockPtr = std::unique_ptr<Block>;

	public:
		LayerLoader() = default;
		~LayerLoader() = default;

		void load(std::vector<Block*>& layer, const std::string& layerFilename, Chipset& chipset);

	private:
		const ChipsetCorrespondanceMapper m_correspondanceMap;
		SDLSurface m_file;

	};
}
