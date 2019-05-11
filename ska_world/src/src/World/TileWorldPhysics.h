#pragma once
#include <list>
#include "Core/Point.h"
#include "MarchingSquare.h"

namespace ska {
	class CollisionProfile;
	
	struct PointArea {
		std::list<Point<int>> pointList;
	};

	std::vector<PointArea> GenerateAgglomeratedTileMap(const std::size_t layerMax, const CollisionProfile& world, const MarchingSquarePredicate& pred = [](const Tile* b) {
		return b != nullptr ? b->collision : TileCollision::No;
	});
	std::vector<Rectangle> GenerateContourTileMap(const std::vector<PointArea>& areas);	
}
