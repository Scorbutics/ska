#include "Data/Events/CollisionEvent.h"
#include "WorldCollisionResponse.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "ECS/EntityManager.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"

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

	if(colX) {
		auto& movementComponent = m_entityManager.getComponent<ska::MovementComponent>(colE.entity);
		movementComponent.vx += wcol.contactX.normal().x * wcol.contactX.overlap().w;
		m_entityManager.addComponent<WorldCollisionComponent>(colE.entity, std::move(wcol));
	}

	if(colY) {
		auto& movementComponent = m_entityManager.getComponent<ska::MovementComponent>(colE.entity);
		movementComponent.vy += wcol.contactY.normal().y * wcol.contactY.overlap().h;
		m_entityManager.addComponent<WorldCollisionComponent>(colE.entity, std::move(wcol));
	}

	return true;
}

ska::WorldCollisionResponse::~WorldCollisionResponse() {
}
