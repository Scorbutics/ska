#include "Path.h"
#include "PathFinder.h"

ska::NodeRefContainer ska::PathFinder::neighbours(const Node& node) {
	NodeRefContainer neighbours;

	const int maxLine = m_container.lineSize();
	const int maxColumn = m_container.empty() ? 0 : m_container.size() / m_container.lineSize();

	addValidNeighbours(neighbours, maxLine, maxColumn, 
		ska::Point<int> {node.column, node.line - 1}, 
		ska::Point<int> {node.column, node.line + 1}, 
		ska::Point<int> {node.column + 1, node.line}, 
		ska::Point<int> {node.column - 1, node.line});

    return neighbours;
}



ska::NodeRefContainer ska::PathFinder::buildPathList(const Point<int>& from, const ska::Point<int>& to) {
	auto& nodeStart = m_container[from.x][from.y];
	const auto& nodeGoal = m_container[to.x][to.y];

	NodeRefContainer pathList;
	NodePriorityRefContainer openList;
	openList.push(nodeStart);
	pathList.push_back(nodeStart);

    while(!openList.empty()) {
		const auto& minPriorityNode = openList.top().get();
		openList.pop();

        if(minPriorityNode == nodeGoal) { break; }

	    const auto& neighbourList = neighbours(minPriorityNode);

        for (auto& neighbourRef : neighbourList) {
			auto& neighbour = neighbourRef.get();

			if(neighbour.isNotAlreadyInPath() || neighbour.willHaveLessCost()) {
				neighbour.calculateGlobalCost(minPriorityNode, nodeGoal);
				openList.push(neighbour);
				pathList.push_back(neighbour);
			}
		}
		
    }

	return pathList;
}

//pour l'explication détaillée de l'A*
//Code inspiré de celui page http://forums.mediabox.fr/wiki/tutoriaux/flashplatform/jeux/pathfinder_algorithme_astar_pratique
//Merci à eux

ska::PathFinder::PathFinder(const Vector2<char>& collisions) {
	const int width = collisions.lineSize();
	const int height = collisions.empty() ? 0 : collisions.size() / collisions.lineSize();
	m_container.reserve(width, height);

	for(auto y = 0; y < height; y++) {
		for(auto x = 0; x < width; x++) {
			const auto& col = collisions[x][y] != 0;
			m_container.push_back(ska::Node {x, y, !col});
		}
	}
}

ska::Path ska::PathFinder::findPath(const Point<int>& from, const Point<int>& to) {
	const auto list = buildPathList(from, to);
	return Path::fromPathList(list);
}
