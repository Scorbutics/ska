#include "Data/Events/CollisionEvent.h"
#include "WorldCollisionResponse.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "ECS/EntityManager.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "../../../ska_ai/AI/IADefinedMovementComponent.h"

ska::WorldCollisionResponse::WorldCollisionResponse(CollisionProfile& cp, GameEventDispatcher& ged, EntityManager& em) :
	WorldCollisionObserver(std::bind(&WorldCollisionResponse::onWorldCollision, this, std::placeholders::_1), ged),
	m_entityManager(em),
	m_collisionProfile(cp) {
}

ska::WorldCollisionResponse::WorldCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, CollisionProfile& cp, GameEventDispatcher& ged, EntityManager& em) :
	WorldCollisionObserver(onEntityCollision, ged),
	m_entityManager(em),
	m_collisionProfile(cp) {
}

bool ska::WorldCollisionResponse::onWorldCollision(CollisionEvent& colE) {
	if (colE.wcollisionComponent == nullptr) {
		return false;
	}

	auto wcol = *colE.wcollisionComponent;
	auto colX = false;
	if (wcol.xaxis) {
		for (const auto& p : wcol.blockColPosX) {
			colX |= !m_collisionProfile.isBlockAuthorizedAtPos(p, colE.collidableComponent.authorizedBlockIds);
			if (colX) {
				auto& movementComponent = m_entityManager.getComponent<ska::MovementComponent>(colE.entity);
				auto& forceComponent = m_entityManager.getComponent<ska::ForceComponent>(colE.entity);
				movementComponent.vx = 0;
				movementComponent.ax = 0;
				forceComponent.x = 0;
				break;
			}
		}
	}

	auto colY = false;
	if (wcol.yaxis) {
		for (const auto& p : wcol.blockColPosY) {
			colY |= !m_collisionProfile.isBlockAuthorizedAtPos(p, colE.collidableComponent.authorizedBlockIds);			
			if (colY) {
				auto& movementComponent = m_entityManager.getComponent<ska::MovementComponent>(colE.entity);
				auto& forceComponent = m_entityManager.getComponent<ska::ForceComponent>(colE.entity);
				movementComponent.vy = 0;
				movementComponent.ay = 0;
				forceComponent.y = 0;
				break;
			}
		}
	}

	auto& cc = m_entityManager.getComponent<CollidableComponent>(colE.entity);
	
	if(colX) {
		SKA_LOG_INFO("Collision. Ghost : ", cc.ghost);
		auto& pc = m_entityManager.getComponent<PositionComponent>(colE.entity);
		pc.x += wcol.contactX.normal().x * wcol.contactX.overlap().w;
		m_entityManager.addComponent<WorldCollisionComponent>(colE.entity, std::move(wcol));
	}

	if(colY) {
		SKA_LOG_INFO("Collision. Ghost : ", cc.ghost);
		auto& pc = m_entityManager.getComponent<PositionComponent>(colE.entity);
		pc.y += wcol.contactY.normal().y * wcol.contactY.overlap().h;
		m_entityManager.addComponent<WorldCollisionComponent>(colE.entity, std::move(wcol));
	}

	return true;
}

ska::WorldCollisionResponse::~WorldCollisionResponse() {
}
