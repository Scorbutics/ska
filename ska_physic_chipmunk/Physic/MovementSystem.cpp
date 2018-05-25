#include "MovementSystem.h"
#include "Space.h"
#include "ECS/Basics/Physic/MovementComponent.h"

ska::cp::MovementSystem::MovementSystem(ska::EntityManager& em, Space& space) :
	System(em),
	m_space(space) {
}

std::pair<bool, bool> ska::cp::MovementSystem::adjustVelocity(const ska::EntityId& entityId, ska::cp::Body& body, float maxVelocity) {
	auto& mc = m_componentAccessor.get<ska::MovementComponent>(entityId);
	auto bodyVelocity = body.getVelocity();
	auto xDone = false;
	auto yDone = false;
	if (NumberUtils::absolute(bodyVelocity.x) > maxVelocity) {
		bodyVelocity.x = bodyVelocity.x > 0 ? maxVelocity : -maxVelocity;
		xDone = true;
	}

	if (NumberUtils::absolute(bodyVelocity.y) > maxVelocity) {
		bodyVelocity.y = bodyVelocity.y > 0 ? maxVelocity : -maxVelocity;
		yDone = true;
	}

	if (xDone && yDone) {
		bodyVelocity.x *= 0.717;
		bodyVelocity.y *= 0.717;
	}

	mc.vx = bodyVelocity.x / 50;
	mc.vy = bodyVelocity.y / 50;

	body.setVelocity(std::move(bodyVelocity));
	return std::make_pair(xDone, yDone);
}

void ska::cp::MovementSystem::refresh(unsigned int ellapsedTime) {
	const auto& entities = getEntities();
	for(const auto& entity : entities) {
		auto& fc = m_componentAccessor.get<ska::ForceComponent>(entity);
		auto& bc = m_componentAccessor.get<ska::cp::HitboxComponent>(entity);
		auto& pc = m_componentAccessor.get<ska::PositionComponent>(entity);

		auto& body = m_space.getBodies()[bc.bodyIndex];

		auto [xDone, yDone] = adjustVelocity(entity, body, fc.maxSpeed);

		body.applyImpulse({xDone ? 0.F : fc.x, yDone ? 0.F : fc.y});
		fc.z = fc.y = fc.x = 0;
		const auto position = body.getPosition();
		const auto& shape = m_space.getShapes()[bc.shapeIndex];
		const auto& entityDimensions = shape.getDimensions();
		pc.x = static_cast<long>(position.x - entityDimensions.w / 2.F);
		pc.y = static_cast<long>(position.y - entityDimensions.h / 2.F);

		const auto rotation = body.getRotation();
		pc.rotationX = rotation.x;
		pc.rotationY = rotation.y;


	}
}
