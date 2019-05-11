#include "Core/Data/Events/CollisionEvent.h"
#include "WorldCollisionResponse.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Core/ECS/EntityManager.h"
#include "EntityCollisionResponse.h"

ska::WorldCollisionResponse::WorldCollisionResponse(ska::TileWorld& world, GameEventDispatcher& ged, EntityManager& em) :
	WorldCollisionObserver(std::bind(&WorldCollisionResponse::onWorldCollision, this, std::placeholders::_1), ged),
	m_entityManager(em),
	m_world(world) {
}

ska::WorldCollisionResponse::WorldCollisionResponse(const std::function<bool(CollisionEvent&)>& onEntityCollision, TileWorld& world, GameEventDispatcher& ged, EntityManager& em) :
	WorldCollisionObserver(onEntityCollision, ged),
	m_entityManager(em),
	m_world(world) {
}

bool ska::WorldCollisionResponse::onWorldCollision(CollisionEvent& colE) {
	if (colE.wcollisionComponent == nullptr) {
		return false;
	}

	auto wcol = *colE.wcollisionComponent;
	auto colX = false;

	//usually 20% to 80%
	static const auto percent = 0.2F;

	//usually 0.01 to 0.1
	static const auto slope = 0.01F;
	/*
	 TODO sans utiliser d'ECS pour simplifier les dialogues par évènements avec le moteur de jeu
	 
	if (wcol.xaxis) {
		for (const auto& p : wcol.blockColPosX) {
			colX |= !m_world.isBlockAuthorizedAtPos(p, colE.collidableComponent.authorizedBlockIds);
			if (colX) {
				auto& movementComponent = m_entityManager.getComponent<ska::MovementComponent>(colE.entity);
				auto& forceComponent = m_entityManager.getComponent<ska::ForceComponent>(colE.entity);

				auto& pc = m_entityManager.getComponent<PositionComponent>(colE.entity);
				auto pcBlock = ska::PositionComponent{ ska::Point<int>(p) };

				EntityCollisionResponse::correctPosition(pc, pcBlock, forceComponent.weight == 0 ? std::numeric_limits<float>::max() : 1.F / forceComponent.weight, 0, wcol.contactX, slope, percent);

				movementComponent.vx = 0;
				break;
			}
		}
	}

	auto colY = false;
	if (wcol.yaxis) {
		for (const auto& p : wcol.blockColPosY) {
			colY |= !m_world.isBlockAuthorizedAtPos(p, colE.collidableComponent.authorizedBlockIds);
			if (colY) {
				auto& movementComponent = m_entityManager.getComponent<ska::MovementComponent>(colE.entity);
				auto& forceComponent = m_entityManager.getComponent<ska::ForceComponent>(colE.entity);

				auto& pc = m_entityManager.getComponent<PositionComponent>(colE.entity);
				auto pcBlock = ska::PositionComponent { ska::Point<int>(p) };

				EntityCollisionResponse::correctPosition(pc, pcBlock, forceComponent.weight == 0 ? std::numeric_limits<float>::max() : 1.F / forceComponent.weight, 0, wcol.contactY, slope, percent);

				movementComponent.vy = 0;
				break;
			}
		}
	}

	auto& cc = m_entityManager.getComponent<CollidableComponent>(colE.entity);

	if(colX) {
		SKA_LOG_INFO("Collision. Ghost : ", cc.ghost);
		m_entityManager.addComponent<WorldCollisionComponent>(colE.entity, std::move(wcol));
	}

	if(colY) {
		SKA_LOG_INFO("Collision. Ghost : ", cc.ghost);
		m_entityManager.addComponent<WorldCollisionComponent>(colE.entity, std::move(wcol));
	}*/

	return true;
}

ska::WorldCollisionResponse::~WorldCollisionResponse() {
}
