
#include "Path.h"
#include "PathFinder.h"

ska::NodeRefContainer ska::PathFinder::neighbours(const Node& node) {
	NodeRefContainer neighbours;

	const int maxLine = m_container.lineSize();
	const int maxColumn = m_container.empty() ? 0 : m_container.size() / m_container.lineSize();
	
	if(node.line - 1 >= 0) {
        neighbours.push_back(m_container[node.column][node.line - 1]);
	}

    if(node.line + 1 < maxLine) {
        neighbours.push_back(m_container[node.column][node.line + 1]);
	}

    if(node.column + 1 < maxColumn) {
        neighbours.push_back(m_container[node.column + 1][node.line]);
	}

    if(node.column - 1 >= 0) {
        neighbours.push_back(m_container[node.column - 1][node.line]);
	}

    return neighbours;
}



std::pair<gsl::not_null<ska::Node*>, ska::NodePriorityRefContainer> ska::PathFinder::buildOpenList(const Point<int>& from, const ska::Point<int>& to) {
	auto nodeStart = m_container[from.x][from.y];
	const auto nodeGoal = m_container[to.x][to.y];

	NodePriorityRefContainer openList;
	openList.push(nodeStart);

    while(!openList.empty()) {
		const auto& minPriorityNode = openList.top().get();

        if(minPriorityNode == nodeGoal) { break; }

	    const auto& neighbourList = neighbours(minPriorityNode);

        for (auto& neighbourRef : neighbourList) {
			auto& neighbour = neighbourRef.get();

			if(neighbour.isNotAlreadyInPath() && neighbour.willHaveLessCost()) {
				neighbour.calculateGlobalCost(minPriorityNode, nodeGoal);
			}
			openList.push(neighbour);
		}
		
    }

	return std::make_pair(&nodeStart, std::move(openList));
}

//pour l'explication détaillée de l'A*
//Code inspiré de celui page http://forums.mediabox.fr/wiki/tutoriaux/flashplatform/jeux/pathfinder_algorithme_astar_pratique
//Merci à eux

ska::PathFinder::PathFinder(const Vector2<int>& collisions) {
	const int width = collisions.lineSize();
	const int height = collisions.empty() ? 0 : collisions.size() / collisions.lineSize();
	m_container.reserve(width, height);

	for(auto x = 0; x < width; x++) {
		for(auto y = 0; y < height; y++) {
			const auto& col = collisions[x][y] != 0;
			m_container.push_back(ska::Node {x, y, col});
		}
	}
}

ska::Path&& ska::PathFinder::findPath(const Point<int>& from, const Point<int>& to) {
	const auto [nodeStart, openList] = buildOpenList(from, to);
	auto path = PathString::fromOpenList(*nodeStart, openList);
	auto type = PathType::Defined;
	return std::move(ska::Path { std::move(path), std::move(type) });
}
