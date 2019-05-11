#pragma once

#include "Core/Point.h"
#include "TileCollision.h"
#include "TileProperties.h"

namespace ska {

	struct Tile {
		Tile() = default;
		Tile(Point<int> posChipset, TileProperties properties = {}, TileCollision col = TileCollision::No);
	
		Point<int> id {};
		TileCollision collision {};
		TileProperties properties {};
	};

}
