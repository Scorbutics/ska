#include "MarchingSquare.h"
#include "Block.h"
#include "Utils/SkaConstants.h"
#include <unordered_set>

std::list<ska::Point<int>> ska::MarchingSquare::apply(const Vector2<Block*>& layer, std::unordered_set<Point<int>>& remainingBlocks) {
	const auto startPoint = getStartingPoint(layer, remainingBlocks);
	std::list<Point<int>> path;

	const auto width = layer.lineSize();
	const auto height = layer.size() / width;

	if(startPoint.x == width && startPoint.y == height) {
		return path;
	}

	auto point = startPoint;
	auto lastDirection = StepDirection::None;
	do {
		if(point.x < width && point.y < height && point.x >= 0 && point.y >= 0) {
			remainingBlocks.erase(point);
			path.push_back(point);
		}
		
		const auto direction = nextStep(layer, point, lastDirection);
		switch(direction) {
		case StepDirection::Down:
			point.y++;
			break;
	
		case StepDirection::Right:
			point.x++;
			break;
		
		case StepDirection::Left:
			point.x--;
			break;
		
		case StepDirection::Up:
			point.y--;
			break;
		
		default:
			break;
		}
		lastDirection = direction;
	} while (point != startPoint);
	return path;
}

ska::Point<int> ska::MarchingSquare::getStartingPoint(const Vector2<Block*>& layer, const std::unordered_set<Point<int>>& in) const {
	const auto width = layer.lineSize();
	const auto height = layer.size() / width;

	for (auto x = 0; x < width; x++) {
		for (auto y = 0; y < height; y++) {
			const auto block = layer[x][y];
			if (block != nullptr && block->getCollision() == BLOCK_COL_YES) {
				if (in.count({ x, y }) > 0) {
					return { x, y };
				}
			}
		}
	}

	return { static_cast<int>(width), static_cast<int>(height) };
}

int GetCollisionBoundChecked(const ska::Vector2<ska::Block*>& layer, const ska::Point<int>& point) {
	const auto width = layer.lineSize();
	const auto height = layer.size() / width;

	if(point.x < width && point.y < height && point.x >= 0 && point.y >= 0) {
		const auto b = layer[point.x][point.y];
		return b != nullptr ? b->getCollision() : BLOCK_COL_NO;
	}
	return BLOCK_COL_NO;
}

ska::StepDirection ska::MarchingSquare::nextStep(const Vector2<Block*>& layer, const Point<int>& point, const StepDirection& previousStep) {
	StepDirection result;

	const auto upLeft = GetCollisionBoundChecked(layer, { point.x - 1, point.y - 1 });
	const auto upRight = GetCollisionBoundChecked(layer, { point.x, point.y - 1 });
	const auto downLeft = GetCollisionBoundChecked(layer, { point.x - 1, point.y });
	const auto downRight = GetCollisionBoundChecked(layer, { point.x, point.y });

	const auto state = upLeft + (upRight << 1) + (downLeft << 2) + (downRight << 3);

	//See the differents states of the marching squares algorithm to explain the code
	switch (state) {
	case 1: result = StepDirection::Up; break;
	case 2: result = StepDirection::Right; break;
	case 3: result = StepDirection::Right; break;
	case 4: result = StepDirection::Left; break;
	case 5: result = StepDirection::Up; break;
	case 6:		
		result = previousStep == StepDirection::Up ? StepDirection::Left : StepDirection::Right;
		break;
	case 7: result = StepDirection::Right; break;
	case 8: result = StepDirection::Down; break;
	case 9:
		result = previousStep == StepDirection::Right ? StepDirection::Up : StepDirection::Down;
		break;
	case 10: result = StepDirection::Down; break;
	case 11: result = StepDirection::Down; break;
	case 12: result = StepDirection::Left; break;
	case 13: result = StepDirection::Up; break;
	case 14: result = StepDirection::Left; break;
	default:
		result = StepDirection::None;
		break;
	}

	return result;
}
