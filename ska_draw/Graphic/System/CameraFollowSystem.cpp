#include "CameraFollowSystem.h"
#include "../GraphicComponent.h"

ska::CameraFollowSystem::CameraFollowSystem(EntityManager& entityManager, GameEventDispatcher& ged, const unsigned int screenW, const unsigned int screenH) : 
	CameraSystem(entityManager, ged, screenW, screenH) {
}

void ska::CameraFollowSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	const auto mainEntityIt = processed.begin();
	if(mainEntityIt != processed.end()) {
		auto entityId = *mainEntityIt;
		auto& pc = m_componentAccessor.get<PositionComponent>(entityId);
		m_follow = {static_cast<int>(pc.x), static_cast<int>(pc.y), 0, 0 };
		m_pos = &m_follow;
		focusOn(m_follow, &entityId);
	}

}

ska::CameraFollowSystem::~CameraFollowSystem()
{
}
