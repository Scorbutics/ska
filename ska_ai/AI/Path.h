#pragma once
#include <string>
#include <queue>
#include "Utils/MovableNonCopyable.h"
#include "Point.h"
#include "Node.h"
#include "Utils/Vector2.h"

namespace ska {

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

	private:
		std::vector<PathDirection> m_path;
		std::size_t m_cursor;
	};

	class Path : public MovableNonCopyable {
	public:
		Path(PathString path, PathType type);

		const PathString path;		
		const PathType type;
	};

}

