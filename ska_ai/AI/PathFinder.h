#pragma once

#include "Utils/Vector2.h"
#include "Path.h"

namespace ska {
	class PathFinder {
	public:
		PathFinder(const Vector2<int>& collisions);
		
		Path&& findPath(const Point<int>& from, const ska::Point<int>& to);

	private:
		std::pair<gsl::not_null<ska::Node*>, NodePriorityRefContainer> buildOpenList(const Point<int>& from, const ska::Point<int>& to);
		NodeRefContainer neighbours(const Node& node);

		NodeOwnerContainer m_container;
	};
}
