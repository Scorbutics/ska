#pragma once
#include <vector>
#include "Rectangle.h"

namespace ska {
	class CollisionProfile;

	enum class TileAgglomerationPriority {
		HORIZONTAL,
		VERTICAL
	};
	
	std::vector<ska::Rectangle> TileAgglomerate(std::size_t layerMax, const ska::CollisionProfile& world, TileAgglomerationPriority priority = TileAgglomerationPriority::HORIZONTAL);
}
