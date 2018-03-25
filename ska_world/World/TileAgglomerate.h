#pragma once
#include <vector>
#include "Rectangle.h"

namespace ska {
	class TileWorld;

	enum class TileAgglomerationPriority {
		HORIZONTAL,
		VERTICAL
	};
	
	std::vector<ska::Rectangle> TileAgglomerate(const ska::TileWorld& world, TileAgglomerationPriority priority = TileAgglomerationPriority::HORIZONTAL);
}
