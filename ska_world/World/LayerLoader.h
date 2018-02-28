#pragma once
#include <memory>
#include "Physic/Block.h"
#include "LayerData.h"

namespace ska {
	class Chipset;
	class ChipsetCorrespondanceMap;

	class LayerLoader {
		using BlockPtr = std::unique_ptr<Block>;

	public:
		LayerLoader() = default;
		~LayerLoader() = default;

		ska::LayerData load(const std::string& layerFilename, Chipset& chipset);

	};
}
