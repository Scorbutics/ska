#include "CameraFixedSystem.h"
#include "../GraphicComponent.h"

ska::CameraFixedSystem::CameraFixedSystem(EntityManager& entityManager, GameEventDispatcher& ged, const unsigned int screenW, const unsigned int screenH, Point<int> point) :
CameraSystem(entityManager, ged, screenW, screenH),
m_fixed({ point.x, point.y, 0, 0}) {
	m_pos = &m_fixed;
}

void ska::CameraFixedSystem::refresh(unsigned int) {
	focusOn(*m_pos, nullptr);
}

ska::CameraFixedSystem::~CameraFixedSystem()
{
}
