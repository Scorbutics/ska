#include "Node.h"
#include "Utils/NumberUtils.h"

ska::Node::Node(int column, int line, bool walkable) :
	column(column),
	line(line),
	walkable(walkable) {
}

void ska::Node::calculateGlobalCost(const Node& parent, const Node& nodeGoal) {
	m_heuristic = ska::NumberUtils::absolute(column - nodeGoal.column) + abs(line - nodeGoal.line);
	m_parent = &parent;
	m_costSoFar = m_parent->m_costSoFar + 1;
	m_priority = m_costSoFar + m_heuristic;
	m_notInPath = false;
}
