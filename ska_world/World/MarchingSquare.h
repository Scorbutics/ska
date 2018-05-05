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

	std::pair<bool, std::list<Point<int>>> MarchingSquare(const CollisionProfile& world, std::unordered_set<Point<int>>& doneBlocks, const MarchingSquarePredicate& pred);
}
