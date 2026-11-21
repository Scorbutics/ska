#include "AI/__internalConfig/LoggerConfig.h"
#include "IAFollowSystem.h"
#include <cmath>
#include <vector>

ska::IAFollowSystem::IAFollowSystem(EntityManager& entityManager) :
	System(entityManager),
	m_entities(entityManager) {
}

void ska::IAFollowSystem::refresh(unsigned int) {
	auto stopped = std::vector<EntityId>{};
	for (const auto entity : getEntities()) {
		const auto& follow = m_componentAccessor.get<IAFollowComponent>(entity);
		if (!m_entities.hasComponent<PositionComponent>(follow.target) || !m_entities.hasComponent<HitboxComponent>(follow.target) || follow.target == entity) {
			stopped.push_back(entity);
			continue;
		}
		// Already walking
		if (m_componentPossibleAccessor.get<IADefinedMovementComponent>(entity) != nullptr) {
			continue;
		}

		const auto center = PositionComponent::getCenterPosition(m_componentAccessor.get<PositionComponent>(entity), m_componentAccessor.get<HitboxComponent>(entity));
		const auto targetCenter = PositionComponent::getCenterPosition(m_entities.getComponent<PositionComponent>(follow.target), m_entities.getComponent<HitboxComponent>(follow.target));
		const auto dx = targetCenter.x - center.x;
		const auto dy = targetCenter.y - center.y;
		const auto distance = std::sqrt(dx * dx + dy * dy);
		if (distance <= follow.distance + Margin) {
			continue;
		}

		// One step : to the follow distance of the target
		const auto walk = distance - follow.distance;
		auto movement = IADefinedMovementComponent{};
		movement.origin = Point<int>{ static_cast<int>(center.x), static_cast<int>(center.y) };
		movement.directions.push_back(Point<int>{ static_cast<int>(dx / distance * walk), static_cast<int>(dy / distance * walk) });
		// Given up after a while when blocked : 500 ms per block of 48 pixels, at least 1 s
		movement.delay = 1000 + static_cast<unsigned int>(walk / 48.F * 500.F);
		m_entities.addComponent<IADefinedMovementComponent>(entity, std::move(movement));
	}

	for (const auto entity : stopped) {
		m_componentAccessor.remove<IAFollowComponent>(entity);
	}
}
