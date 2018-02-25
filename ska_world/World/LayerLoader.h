#pragma once
#include <memory>
#include "Physic/Block.h"

namespace ska {
	class Chipset;
	class ChipsetCorrespondanceMap;
	template <class T> class Vector2;

	class LayerLoader {
		using BlockPtr = std::unique_ptr<Block>;

	public:
		LayerLoader() = default;
		~LayerLoader() = default;

		void load(Vector2<Block*>& layer, const std::string& layerFilename, Chipset& chipset);

	};
}
