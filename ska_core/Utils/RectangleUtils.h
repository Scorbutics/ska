#pragma once

#include "../Rectangle.h"
#include "../Point.h"
#include <optional>

namespace ska {
	class RectangleUtils {
	public:
		~RectangleUtils() = default;

		template <class T>
		static T distanceSquared(const Point<T>& pos1, const Point<T>& pos2) {
			return (((pos1.x - pos2.x) * (pos1.x - pos2.x)) + ((pos1.y - pos2.y) * (pos1.y - pos2.y)));	
		}

		template <class T>
		static bool isPositionInBox(const Point<T> &pos, const Rectangle &box) {
			return((pos.x >= box.x) && (pos.x < box.x + box.w) && (pos.y >= box.y) && (pos.y < box.y + box.h));
		}

		static Rectangle unionRect(const Rectangle& intersection, const Rectangle& overlap);
		static Rectangle intersect(const Rectangle& r1, const Rectangle& r2);
		static Rectangle posToCenterPicture(const Rectangle& imageToCenter, const Rectangle& imageBackground);
		static bool collisionBoxABoxB(const Rectangle& rectA, const Rectangle& rectB);
		static int getDirectionFromPos(const Point<float>& center, const Point<float>& destination);
		
		template <class T>
		static ska::Rectangle createRectangleFromPoints(const Point<T>& p1, const Point<T>& p2) {
			const int width = ska::NumberUtils::absolute(p1.x - p2.x);
			const int height = ska::NumberUtils::absolute(p1.y - p2.y);
			auto x = ska::NumberUtils::minimum(p1.x, p2.x);
			auto y = ska::NumberUtils::minimum(p1.y, p2.y);
			return ska::Rectangle {x, y, width, height};
		}

		template <class T>
		static T projection(const Point<T>& vector, const Point<T>& axis) {
			return vector.x * axis.x + vector.y * axis.y;
		}

		static std::vector<Point<int>> createIntermediatePoints(const Point<int>& pointA, const Point<int>& pointB, bool step = 1);

	private:
		RectangleUtils();

	};
}
