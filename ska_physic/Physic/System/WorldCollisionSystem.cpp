#include "WorldCollisionSystem.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Physic/CollisionContact.h"

#include "Utils/RectangleUtils.h"

ska::WorldCollisionSystem::WorldCollisionSystem(EntityManager& entityManager, CollisionProfile& cp, GameEventDispatcher& ged) :
	System(entityManager),
	m_collisionProfile(cp),
	m_ged(ged) {
}

ska::Rectangle ska::WorldCollisionSystem::calculateOverlap(Rectangle nextPos, const std::vector<Point<int>>& points) {
	//TODO taille bloc dynamique
	constexpr auto blockSize = 48;

	if(points.empty()) {
		return Rectangle{0, 0, 0, 0};
	}

	Rectangle summedBlocksToOverlap;
	auto first = true;
	for (auto& p : points) {
		const auto currentRect = Rectangle{ p.x, p.y, blockSize, blockSize };
		if (first) {
			summedBlocksToOverlap = currentRect;
			first = false;
		} else {
			summedBlocksToOverlap = RectangleUtils::unionRect(summedBlocksToOverlap, currentRect);
		}
	}

	return RectangleUtils::intersect(nextPos, summedBlocksToOverlap);
}


void ska::WorldCollisionSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		const auto& cc = m_componentAccessor.get<CollidableComponent>(entityId);
		if (cc.ghost) {
			continue;
		}

		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> lastBlockColPosY;
		const auto& wccPtr = m_componentPossibleAccessor.get<WorldCollisionComponent>(entityId);
		if (wccPtr != nullptr) {
			const auto& wcc = *wccPtr;
			lastBlockColPosX = wcc.blockColPosX;
			lastBlockColPosY = wcc.blockColPosY;
			//SKA_LOG_DEBUG("Suppression collision monde ", entityId);
			m_componentAccessor.remove<WorldCollisionComponent>(entityId);
			/* Il faut rafra�chir l'ensemble des syst�mes directement ici (au travers de l'EntityManager),
			car il est possible de cr�er un autre WorldCollisionComponent dans le m�me tour de boucle.
			Si le refresh n'est pas fait, il sera supprim� par le prochain rafra�chissement de l'EntityManager */
			refreshEntity(entityId);
		}

		const auto entityHitbox = createHitBox(entityId, false, true);

		WorldCollisionComponent wcol;
		auto collided = false;
		const Rectangle nextPos = { entityHitbox.x, entityHitbox.y, entityHitbox.w, entityHitbox.h };

		wcol.blockColPosX.clear();
		wcol.blockColPosY.clear();
		const auto& intersect = m_collisionProfile.intersectBlocksAtPos(nextPos, wcol.blockColPosX, wcol.blockColPosY);
		if (intersect) {
			wcol.xaxis = !wcol.blockColPosX.empty();
			wcol.yaxis = !wcol.blockColPosY.empty();
			wcol.lastBlockColPosX = lastBlockColPosX;
			wcol.lastBlockColPosY = lastBlockColPosY;
			const auto& overlapX = calculateOverlap(nextPos, wcol.blockColPosX);
			const auto& overlapY = calculateOverlap(nextPos, wcol.blockColPosY);
			//SKA_LOG_MESSAGE("Overlap x summed : \t", overlapX.x, ";", overlapX.y, ";", overlapX.w, ";", overlapX.h);
			//SKA_LOG_MESSAGE("Overlap y summed : \t", overlapY.x, ";", overlapY.y, ";", overlapY.w, ";", overlapY.h);
			wcol.contactX = CollisionContact{ overlapX, nextPos, overlapX };
			wcol.contactY = CollisionContact{ overlapY, nextPos, overlapY };
			if(wcol.contactY.hasCollision() || wcol.contactX.hasCollision()) {
				collided = true;
			}
		}

		if (collided) {
			CollisionEvent ce(entityId, &wcol, nullptr, m_componentAccessor.get<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
		}

	}
}

ska::Rectangle ska::WorldCollisionSystem::createHitBox(EntityId entityId, bool , bool ) const{
	auto& positionComponent = m_componentAccessor.get<PositionComponent>(entityId);
	auto& hitboxComponent = m_componentAccessor.get<HitboxComponent>(entityId);
	auto& movementComponent = m_componentAccessor.get<MovementComponent>(entityId);

	Rectangle hitBox;
	hitBox.x = ska::NumberUtils::round(positionComponent.x + movementComponent.vx + movementComponent.ax + hitboxComponent.xOffset);
	hitBox.y = ska::NumberUtils::round(positionComponent.y + movementComponent.vy + movementComponent.ay + hitboxComponent.yOffset);
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

ska::WorldCollisionSystem::~WorldCollisionSystem() {
}
