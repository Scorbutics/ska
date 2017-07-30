#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"

ska::CollisionSystem::CollisionSystem(EntityManager& entityManager, CollisionProfile& cp, GameEventDispatcher& ged) :
	System(entityManager),
	m_collisionProfile(cp),
	m_ged(ged) {
}

void ska::CollisionSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {

		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> lastBlockColPosY;
		const auto& wccPtr = m_componentPossibleAccessor.get<WorldCollisionComponent>(entityId);
		if (wccPtr != nullptr) {
			const auto& wcc = *wccPtr;
			lastBlockColPosX = wcc.blockColPosX;
			lastBlockColPosY = wcc.blockColPosY;
			m_componentAccessor.remove<WorldCollisionComponent>(entityId);
			/* Il faut rafraîchir l'ensemble des systèmes directement ici (au travers de l'EntityManager),
			car il est possible de créer un autre WorldCollisionComponent dans le même tour de boucle.
			Si le refresh n'est pas fait, il sera supprimé par le prochain rafraîchissement de l'EntityManager */
			refreshEntity(entityId);
		}

		const auto entityHitbox = createHitBox(entityId, false, true);

		auto entityCollided = false;
		CollisionComponent col;
		for (auto itEntity : processed) {
			if (itEntity != entityId) {
				const auto& intersection = RectangleUtils::intersect(entityHitbox, createHitBox(itEntity, true, true));
				if (intersection.w != 0 && intersection.h != 0) {
					col.origin = entityId;
					col.target = itEntity;
					entityCollided = true;
					col.xaxis = intersection.h > 1;
					col.yaxis = intersection.w > 1;
					col.overlap = ska::Rectangle{ 0, 0, intersection.w, intersection.h };
				}

			}

			if (entityCollided) {
				break;
			}

		}

		WorldCollisionComponent wcol;
		auto collided = false;
		Rectangle nextPos = { entityHitbox.x, entityHitbox.y, entityHitbox.w, entityHitbox.h };

		wcol.blockColPosX.clear();
		wcol.blockColPosY.clear();
		const auto& intersect = m_collisionProfile.intersectBlocksAtPos(nextPos, wcol.blockColPosX, wcol.blockColPosY);
		if (intersect) {
			collided = true;
			wcol.xaxis = true;
			wcol.yaxis = true;
			wcol.lastBlockColPosX = lastBlockColPosX;
			wcol.lastBlockColPosY = lastBlockColPosY;
		}

		if (collided) {
			CollisionEvent ce(entityId, &wcol, nullptr, m_componentAccessor.get<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
		}

		if (entityCollided) {
			/* When collision between entities is detected, we can do things as decreasing health,
			pushing entities, or any statistic interaction */
			CollisionEvent ce(entityId, nullptr, &col, m_componentAccessor.get<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
		}
	}
}

ska::Rectangle ska::CollisionSystem::createHitBox(EntityId entityId, bool xaxis, bool noMove) const{
	auto& positionComponent = m_componentAccessor.get<PositionComponent>(entityId);
	auto& hitboxComponent = m_componentAccessor.get<HitboxComponent>(entityId);
	auto& movementComponent = m_componentAccessor.get<MovementComponent>(entityId);

	Rectangle hitBox;
	hitBox.x = ska::NumberUtils::round(positionComponent.x + movementComponent.vx + movementComponent.ax + hitboxComponent.xOffset);
	hitBox.y = ska::NumberUtils::round(positionComponent.y + movementComponent.vy + movementComponent.ay + hitboxComponent.yOffset);
	hitBox.w = hitboxComponent.width + 1;
	hitBox.h = hitboxComponent.height + 1;
	return hitBox;
}

ska::CollisionSystem::~CollisionSystem() {
}
