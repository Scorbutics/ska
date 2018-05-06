#pragma once
#include <list>
#include "Point.h"

namespace ska {
	class CollisionProfile;

	struct PointArea {
		std::list<Point<int>> pointList;
	};

	std::vector<PointArea> GenerateAgglomeratedTileMap(const std::size_t layerMax, const CollisionProfile& world);
	std::vector<Rectangle> GenerateContourTileMap(const std::vector<PointArea>& areas);	
}
