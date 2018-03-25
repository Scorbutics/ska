#pragma once
#include <list>
#include <unordered_set>
#include "Point.h"
#include "Physic/Block.h"

namespace ska {
	class TileWorld;

	enum class StepDirection {
		Up,
		Left,
		Down,
		Right,
		None
	};

	using MarchingSquarePredicate = std::function<ska::BlockCollision(const Block*)>;	

	std::pair<bool, std::list<ska::Point<int>>> MarchingSquare(const ska::TileWorld& world, std::unordered_set<Point<int>>& remainingBlocks, const ska::MarchingSquarePredicate& pred);
}
