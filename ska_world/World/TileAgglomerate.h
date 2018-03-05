#pragma once
#include <vector>
#include "Rectangle.h"

namespace ska {
	class TileWorld;

	class TileAgglomerate {
	public: 
		TileAgglomerate() = delete;
		~TileAgglomerate() = default;
		
		static std::vector<ska::Rectangle> apply(const ska::TileWorld& world, bool horizontal);
	};
}
