#pragma once

#include "Base/Containers/Vector2.h"
#include "Core/Point.h"
#include "Path.h"

namespace ska {
	class PathFinder {
	public:
		PathFinder(const Vector2<char>& collisions);
		
		Path findPath(const Point<int>& from, const ska::Point<int>& to);

	private:
		NodeRefContainer buildPathList(const Point<int>& from, const ska::Point<int>& to);
		NodeRefContainer neighbours(const Node& node);

		template<class Point>
		void addValidNeighbour(ska::NodeRefContainer& neighbours, int maxLine, int maxColumn, Point p) {
			if(p.x >= 0 && p.y >= 0 && p.x < maxColumn && p.y < maxLine) {
				auto& neighbour = m_container[p.x][p.y];
				if(neighbour.walkable) {
					neighbours.push_back(neighbour);
				}
			}
		}

		template<class ... Points>
		void addValidNeighbours(ska::NodeRefContainer& neighbours, int maxLine, int maxColumn, Points...p) {
			int _[] = { 0, (addValidNeighbour<Points>(neighbours, maxLine, maxColumn, p) , 0)... };
			(void)_;
		}

		NodeOwnerContainer m_container;
	};
}
