#include <string>
#include <vector>

#include "Path.h"
#include "Node.h"
#include "Task/Task.h"

ska::PathString::PathString(const std::string& path) {
	m_path.reserve(path.size());
	for(const auto& p : path) {
		switch(p) {
		case 'h':
			m_path.push_back(PathDirection::Up);
			break;

		case 'b':
			m_path.push_back(PathDirection::Down);
			break;

		case 'g':
			m_path.push_back(PathDirection::Left);
			break;

		case 'd':
			m_path.push_back(PathDirection::Right);
			break;

		default:
			break;
		}
	}
}

ska::PathDirection ska::PathString::getPathDirection(std::size_t number) const {
    if(m_cursor >= m_path.size()) {
        return PathDirection::Unknown;
	}

    return m_path[number];
}

void ska::PathString::reverse() {
	std::reverse(m_path.begin(), m_path.end());
}

ska::PathString ska::PathString::fromOpenList(const Node& start, const NodePriorityRefContainer& openList) {
	std::string path;
		
	const auto *itNode = &openList.top().get();
    while(itNode != &start) {
		const auto directionX = itNode->column - itNode->parent().column + 1;
		const auto directionY = itNode->line - itNode->parent().line + 1;
		
		if(directionX == 1) {
			path += "d";
		} else if(directionX == -1) {
			path += "g";
		}
		
		if(directionY == 1) {
			path += "b";
		} else if (directionY == -1) {
			path += "h";
		}

        itNode = &itNode->parent();
    }

	auto pathString = ska::PathString { path };
	pathString.reverse();
	return pathString;
}

ska::Path::Path(PathString path, PathType type) :
	path(std::move(path)),
	type(type) {
}
