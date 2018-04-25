#include <unordered_set>
#include "MarchingSquare.h"
#include "TileWorld.h"

ska::Point<int> MarchingSquareGetStartingPoint(const ska::TileWorld& world, const std::unordered_set<ska::Point<int>>& in, const ska::MarchingSquarePredicate& pred);
ska::StepDirection MarchingSquareNextStep(const ska::TileWorld& world, const ska::Point<int>& point, const ska::MarchingSquarePredicate& pred, const ska::StepDirection& previousStep);
void ReorganizeFirstPointToBeAlwaysTopLeft(std::list<ska::Point<int>>& path);

std::pair<bool, std::list<ska::Point<int>>> ska::MarchingSquare(const ska::TileWorld& world, std::unordered_set<ska::Point<int>>& doneBlocks, const ska::MarchingSquarePredicate& pred) {
	const auto startPoint = MarchingSquareGetStartingPoint(world, doneBlocks, pred);
	std::list<ska::Point<int>> path;

	const auto width = world.getBlocksX();
	const auto height = world.getBlocksY();

	if(startPoint.x == width && startPoint.y == height) {
		return {true, path};
	}

	auto point = startPoint;
	auto lastDirection = ska::StepDirection::None;
	do {
		if(point.x < width && point.y < height && point.x >= 0 && point.y >= 0) {
			doneBlocks.insert(point);
			path.push_back(point * world.getBlockSize());
		}
		
		const auto direction = MarchingSquareNextStep(world, point, pred, lastDirection);
		switch(direction) {
		case ska::StepDirection::Down:
			point.y++;
			break;
	
		case ska::StepDirection::Right:
			point.x++;
			break;
		
		case ska::StepDirection::Left:
			point.x--;
			break;
		
		case ska::StepDirection::Up:
			point.y--;
			break;
		
		default:
			break;
		}
		lastDirection = direction;
	} while (point != startPoint);
	
	ReorganizeFirstPointToBeAlwaysTopLeft(path);
	return {false, path};
}

void ReorganizeFirstPointToBeAlwaysTopLeft(std::list<ska::Point<int>>& path) {
	if(!path.empty()) {
		auto& lastPoint = path.back();
		auto& firstPoint = path.front();
		if((lastPoint.y == firstPoint.y && lastPoint.x < firstPoint.x)
			|| (lastPoint.x == firstPoint.x && lastPoint.y < firstPoint.y)) {
			auto it = path.cend();
			--it;
			path.push_front(*it);
			path.erase(it);
		}
	}
}

ska::Point<int> MarchingSquareGetStartingPoint(const ska::TileWorld& world, const std::unordered_set<ska::Point<int>>& in, const ska::MarchingSquarePredicate& pred) {
	const int width = world.getBlocksX();
	const int height = world.getBlocksY();

	for (auto x = 0; x < width; x++) {
		for (auto y = 0; y < height; y++) {
			const auto block = world.getHighestBlock(x, y);
			if(pred(block) == ska::TileCollision::Yes) {
				if (in.count({ x, y }) == 0) {
					return { x, y };
				}
			}
		}
	}

	return { static_cast<int>(width), static_cast<int>(height) };
}

ska::TileCollision GetCollisionBoundChecked(const ska::TileWorld& world, const ska::Point<int>& point, const ska::MarchingSquarePredicate& pred) {
	const int width = world.getBlocksX();
	const int height = world.getBlocksY();

	if(point.x < width && point.y < height && point.x >= 0 && point.y >= 0) {
		const auto b = world.getHighestBlock(point.x, point.y);
		return pred(b);
	}
	return ska::TileCollision::No;
}

ska::StepDirection MarchingSquareNextStep(const ska::TileWorld& world, const ska::Point<int>& point, const ska::MarchingSquarePredicate& pred, const ska::StepDirection& previousStep) {
	ska::StepDirection result;

	const auto upLeft = GetCollisionBoundChecked(world, { point.x - 1, point.y - 1 }, pred) != ska::TileCollision::No;
	const auto upRight = GetCollisionBoundChecked(world, { point.x, point.y - 1 }, pred) != ska::TileCollision::No;
	const auto downLeft = GetCollisionBoundChecked(world, { point.x - 1, point.y }, pred) != ska::TileCollision::No;
	const auto downRight = GetCollisionBoundChecked(world, { point.x, point.y }, pred) != ska::TileCollision::No;

	const auto state = upLeft + (upRight << 1) + (downLeft << 2) + (downRight << 3);

	//See the differents states of the marching squares algorithm to explain the code
	switch (state) {
	case 1: result = ska::StepDirection::Up; break;
	case 2: result = ska::StepDirection::Right; break;
	case 3: result = ska::StepDirection::Right; break;
	case 4: result = ska::StepDirection::Left; break;
	case 5: result = ska::StepDirection::Up; break;
	case 6:		
		result = previousStep == ska::StepDirection::Up ? ska::StepDirection::Left : ska::StepDirection::Right;
		break;
	case 7: result = ska::StepDirection::Right; break;
	case 8: result = ska::StepDirection::Down; break;
	case 9:
		result = previousStep == ska::StepDirection::Right ? ska::StepDirection::Up : ska::StepDirection::Down;
		break;
	case 10: result = ska::StepDirection::Down; break;
	case 11: result = ska::StepDirection::Down; break;
	case 12: result = ska::StepDirection::Left; break;
	case 13: result = ska::StepDirection::Up; break;
	case 14: result = ska::StepDirection::Left; break;
	default:
		result = ska::StepDirection::None;
		break;
	}

	return result;
}
