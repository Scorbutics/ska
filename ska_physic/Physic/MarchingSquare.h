#pragma once
#include <list>
#include "Point.h"
#include "Utils/Vector2.h"


namespace ska {
	class Block;

	enum class StepDirection {
		Up,
		Left,
		Down,
		Right,
		None
	};

	class MarchingSquare {
	public: 
		MarchingSquare() = default;
		~MarchingSquare() = default;
		
		std::list<Point<int>> apply(const Vector2<Block*>& layer);

	private:
		Point<int> getStartingPoint(const Vector2<Block*>& layer) const;
		StepDirection nextStep(const Vector2<Block*>& layer, const Point<int>& point, const StepDirection& previousStep);
	};
}
