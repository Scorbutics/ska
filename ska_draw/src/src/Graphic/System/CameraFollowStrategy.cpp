#include "Graphic/__internalConfig/LoggerConfig.h"
#include "CameraFollowStrategy.h"

ska::CameraFollowStrategy::CameraFollowStrategy(ska::EntityManager & em) :
	m_entityManager(em) {
}

std::pair<ska::Rectangle, const ska::EntityId*> ska::CameraFollowStrategy::focus(const std::unordered_set<EntityId>& processed) {
	const auto mainEntityIt = processed.begin();
	if(mainEntityIt != processed.end()) {
		// The entity in the set (not a copy : the pointer is used after this function returns)
		const auto& entityId = *mainEntityIt;
		const auto& pc = m_entityManager.getComponent<PositionComponent>(entityId);
		return std::make_pair(Rectangle {static_cast<int>(pc.x), static_cast<int>(pc.y), 0, 0 }, &entityId);
	}
	return std::make_pair(Rectangle{}, nullptr);	
}