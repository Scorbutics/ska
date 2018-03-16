#include "CollisionSystem.h"
#include "World/CollisionProfile.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/CollisionComponent.h"

ska::CollisionSystem::CollisionSystem(EntityManager& entityManager, GameEventDispatcher& ged) :
	System(entityManager),
	m_ged(ged) {
}

void ska::CollisionSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityIdIt = processed.begin(); entityIdIt != processed.end(); ++entityIdIt) {
		auto entityId = *entityIdIt;
		const auto entityHitbox = createHitBox(entityId);


		for (auto itEntityIt = entityIdIt; itEntityIt != processed.end(); ++itEntityIt) {
			auto itEntity = *itEntityIt;
			if (itEntity != entityId) {
				const auto& otherEntityBox = createHitBox(itEntity);
				const auto& intersection = RectangleUtils::intersect(entityHitbox, otherEntityBox);
				if (intersection.w != 0 && intersection.h != 0) {
					const auto& cc = m_componentAccessor.get<CollidableComponent>(entityId);
					if (!cc.ghost) {
						CollisionComponent col;
						col.origin = entityId;
						col.target = itEntity;
						col.xaxis = intersection.h > 1;
						col.yaxis = intersection.w > 1;
						col.contact = CollisionContact(intersection, entityHitbox, otherEntityBox);

						// When collision between entities is detected, we can do things as decreasing health,
						//pushing entities, or any statistic interaction
						CollisionEvent ce(entityId, nullptr, &col, cc);
						m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
					}
				}

			}

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
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

ska::CollisionSystem::~CollisionSystem() {
}
