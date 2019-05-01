#include "CameraFixedStrategy.h"

ska::CameraFixedStrategy::CameraFixedStrategy(const Point<int>& pos) :
	m_pos(pos) {
}

std::pair<ska::Rectangle, ska::EntityId*> ska::CameraFixedStrategy::focus(const std::unordered_set<EntityId>& processed) {
	return std::make_pair<ska::Rectangle, ska::EntityId*>({ m_pos.x, m_pos.y, 0, 0 }, nullptr);
}
