#include "Tile.h"

ska::Tile::Tile(Point<int> posChipset, const TileProperties properties, const TileCollision col) :
	id(std::move(posChipset)),
	collision(col),
	properties(properties) {
}

