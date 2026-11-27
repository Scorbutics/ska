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

	// The blocks of the predicate, filled : a few rectangles cover them all, without overlapping (the runs of a line, then
	// the runs of the same columns on the next lines are merged). Unlike the contours, nothing can be inside. Only the solid
	// parts of the blocks (Tile::mask : Tile::MaskCells x Tile::MaskCells cells a block, the union of the layers), the
	// rectangles are made of cells.
	std::vector<Rectangle> GenerateFilledTileMap(std::size_t layerMax, const CollisionProfile& world, const MarchingSquarePredicate& pred = [](const Tile* b) {
		return b != nullptr ? b->collision : TileCollision::No;
	});
}
