#include "Data/Events/CollisionEvent.h"
#include "DebugWorldCollisionResponse.h"
#include "ECS/Basics/Graphic/DebugGraphicComponent.h"
#include "ECS/EntityManager.h"
#include "CollisionSystem.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"

ska::DebugWorldCollisionResponse::DebugWorldCollisionResponse(GameEventDispatcher& ged, EntityManager& em) :
WorldCollisionObserver(std::bind(&DebugWorldCollisionResponse::onWorldCollision, this, std::placeholders::_1), ged),
	m_entityManager(em){
}

bool ska::DebugWorldCollisionResponse::onWorldCollision(CollisionEvent& e) const{
	if(e.wcollisionComponent == nullptr) {
		return true;
	}

	DebugGraphicComponent dgc;
	dgc.typeMask = COLLISION;
	auto& wcol = *e.wcollisionComponent;

	if (wcol.xaxis) {
		dgc.blockColPosX = wcol.blockColPosX;
	}

	if (wcol.yaxis) {
		dgc.blockColPosY = wcol.blockColPosY;
	}

	m_entityManager.addComponent<DebugGraphicComponent>(e.entity, std::move(dgc));
	return true;
}

ska::DebugWorldCollisionResponse::~DebugWorldCollisionResponse() {
}
