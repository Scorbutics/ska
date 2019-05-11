#include <string>
#include <vector>

#include "Path.h"
#include "Node.h"
#include "Base/Task/Task.h"

std::vector<ska::PathDirection> ska::Path::buildPath(const std::string& path) {
	auto result = std::vector<ska::PathDirection> {};
	result.reserve(path.size());
	for(auto it = path.crbegin(); it != path.crend(); ++it) {
		const auto& p = *it;
		switch(p) {
		case 'h':
			result.push_back(ska::PathDirection::Up);
			break;

		case 'b':
			result.push_back(ska::PathDirection::Down);
			break;

		case 'g':
			result.push_back(ska::PathDirection::Left);
			break;

		case 'd':
			result.push_back(ska::PathDirection::Right);
			break;

		default:
			break;
		}
	}
	return result;
}

ska::Path ska::Path::fromPathList(const NodeRefContainer& pathList) {
	std::string path;
	if(!pathList.empty()) {
		for(const auto * itNode = &pathList.crbegin()->get(); itNode != &itNode->parent() && *itNode != pathList[0]; itNode = &itNode->parent()) {
			auto& node = *itNode;
			const auto directionX = node.column - node.parent().column;
			const auto directionY = node.line - node.parent().line;
		
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
		}
	}
	return { buildPath(path), PathType::Defined };
}

ska::Path::Path(std::vector<ska::PathDirection> path, PathType type) :
	path(std::move(path)),
	type(type) {
}
