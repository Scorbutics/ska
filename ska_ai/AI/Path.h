#pragma once
#include <string>
#include <unordered_set>
#include "Rectangle.h"
#include "Utils/MovableNonCopyable.h"
#include "Point.h"
#include "Node.h"
#include "Utils/Vector2.h"
#include <queue>

namespace ska {
	class PathGraphe;

	enum class PathType {
		Random,
		Defined, 
		Fixed
	};

	enum class PathDirection {
		Down,
		Right,
		Up,
		Left,
		Unknown
	};

	using NodeRef = std::reference_wrapper<Node>;
	using NodePriorityRefContainer = std::priority_queue<NodeRef, std::vector<NodeRef>, Node::CompareDist>;
	using NodeRefContainer = std::vector<NodeRef>;
	using NodeOwnerContainer = Vector2<Node>;

	class PathString : public MovableNonCopyable {
	public:
		PathString(const std::string& path);
	
		PathDirection getPathDirection(std::size_t number) const;
		void reverse();
		static ska::PathString fromOpenList(const Node& start, const NodePriorityRefContainer& openList);
		PathDirection buildNextPathDirection();

	private:
		std::vector<PathDirection> m_path;
		std::size_t m_cursor;
	};

	class Path : public MovableNonCopyable {
	public:
		Path(PathString path, PathType type);

		const PathString m_path;		
		const PathType m_type;
	};

	class PathFinder {
	public:
		PathFinder(const Vector2<bool>& collisions);
		
		Path&& findPath(const Point<int>& from, const ska::Point<int>& to);

	private:
		std::pair<gsl::not_null<ska::Node*>, NodePriorityRefContainer> buildOpenList(const Point<int>& from, const ska::Point<int>& to);
		NodeRefContainer neighbours(const Node& node) const;

		NodeOwnerContainer m_container;
	};
}

