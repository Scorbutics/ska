#pragma once
#include "Point.h"

namespace ska {
	class TileWorld;

	struct PointArea {
		std::list<Point<int>> pointList;
	};

	std::vector<PointArea> GenerateAgglomeratedTileMap(const ska::TileWorld& world);
	std::vector<Rectangle> GenerateContourTileMap(const std::vector<PointArea>& areas);
}
