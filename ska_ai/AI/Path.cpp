#include <string>
#include <vector>

#include "Path.h"
#include "Node.h"
#include "Task/Task.h"
#include "Point.h"

#define DOWN 0
#define RIGHT 1
#define UP 2
#define LEFT 3

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
	m_path(std::move(path)),
	m_type(type) {
}

/*
ska::NodeRefContainer ska::PathGraphe::neighbours(const Node& node) const {
	int currentPos[4];

	ska::NodeRefContainer neighbours;

    currentPos[DOWN] = n->getLine() + 1;
    currentPos[RIGHT] = n->getCol() + 1;
    currentPos[UP] = n->getLine() - 1;
    currentPos[LEFT] = n->getCol() - 1;

	
	if(currentPos[UP] >= 0)
        neighbours.push_back(&(graphe[n->getCol()][currentPos[UP]]));

    if(currentPos[DOWN] < w.getNbrBlocY())
        neighbours.push_back(&(graphe[n->getCol()][currentPos[DOWN]]));

    if(currentPos[RIGHT] < w.getNbrBlocX())
        neighbours.push_back(&(graphe[currentPos[RIGHT]][n->getLine()]));

    if(currentPos[LEFT] >= 0)
        neighbours.push_back(&(graphe[currentPos[LEFT]][n->getLine()]));
	

    return neighbours;
}
*/


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

ska::Path&& ska::PathFinder::findPath(const Point<int>& from, const Point<int>& to) {
	const auto [nodeStart, openList] = buildOpenList(from, to);
	auto path = PathString::fromOpenList(*nodeStart, openList);
	auto type = PathType::Defined;
	return std::move(ska::Path { std::move(path), std::move(type) });
}




