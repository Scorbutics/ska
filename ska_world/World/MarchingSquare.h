#pragma once
#include <list>
#include <unordered_set>
#include "Point.h"
#include "Tile.h"

namespace ska {
	class CollisionProfile;

	enum class StepDirection {
		Up,
		Left,
		Down,
		Right,
		None
	};

	using MarchingSquarePredicate = std::function<TileCollision(const Tile*)>;

	std::pair<Point<int>, std::list<Point<int>>> MarchingSquare(std::size_t layerMax, const CollisionProfile& world, std::unordered_set<Point<int>>& doneBlocks, const MarchingSquarePredicate& pred, const Point<int>& lastStartPoint);
}
