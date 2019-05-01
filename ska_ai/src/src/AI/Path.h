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

	class Path : public MovableNonCopyable {
	public:		
		static Path fromPathList(const NodeRefContainer& pathList);

		Path(Path&&) = default;
		~Path() = default;

		const std::vector<PathDirection> path;
		const PathType type;

	private:
		static std::vector<PathDirection> buildPath(const std::string& path);
		Path(std::vector<PathDirection> path, PathType type);
	};

}

