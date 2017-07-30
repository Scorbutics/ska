#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/CollisionComponent.h"

ska::CollisionSystem::CollisionSystem(EntityManager& entityManager, GameEventDispatcher& ged) :
	System(entityManager),
	m_ged(ged) {
}

void ska::CollisionSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {

		const auto entityHitbox = createHitBox(entityId);

		auto entityCollided = false;
		CollisionComponent col;
		for (auto itEntity : processed) {
			if (itEntity != entityId) {
				const auto& intersection = RectangleUtils::intersect(entityHitbox, createHitBox(itEntity));
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

		if (entityCollided) {
			/* When collision between entities is detected, we can do things as decreasing health,
			pushing entities, or any statistic interaction */
			CollisionEvent ce(entityId, nullptr, &col, m_componentAccessor.get<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
		}
	}
}

ska::Rectangle ska::CollisionSystem::createHitBox(EntityId entityId) const{
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
