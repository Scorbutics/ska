#include "CameraFollowStrategy.h"

ska::CameraFollowStrategy::CameraFollowStrategy(ska::EntityManager & em) :
	m_entityManager(em) {
}

std::pair<ska::Rectangle, ska::EntityId*> ska::CameraFollowStrategy::focus(const std::unordered_set<EntityId>& processed) {
	const auto mainEntityIt = processed.begin();
	if(mainEntityIt != processed.end()) {
		auto entityId = *mainEntityIt;
		const auto& pc = m_entityManager.getComponent<PositionComponent>(entityId);
		return std::make_pair(Rectangle {static_cast<int>(pc.x), static_cast<int>(pc.y), 0, 0 }, &entityId);
	}
	return std::make_pair(Rectangle{}, nullptr);	
}