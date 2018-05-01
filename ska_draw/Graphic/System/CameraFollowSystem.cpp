#include "CameraFollowSystem.h"
#include "../GraphicComponent.h"

ska::CameraFollowSystem::CameraFollowSystem(EntityManager& entityManager, GameEventDispatcher& ged, const unsigned int screenW, const unsigned int screenH) : 
	CameraSystem(entityManager, ged, screenW, screenH) {
}

void ska::CameraFollowSystem::refresh(unsigned int) {


}

ska::CameraFollowSystem::~CameraFollowSystem()
{
}
