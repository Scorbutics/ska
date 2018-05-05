#pragma once
#include <vector>
#include "Rectangle.h"

namespace ska {
	class CollisionProfile;

	enum class TileAgglomerationPriority {
		HORIZONTAL,
		VERTICAL
	};
	
	std::vector<ska::Rectangle> TileAgglomerate(const ska::CollisionProfile& world, TileAgglomerationPriority priority = TileAgglomerationPriority::HORIZONTAL);
}
