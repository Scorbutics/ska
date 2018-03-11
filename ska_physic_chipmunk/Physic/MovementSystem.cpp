#include "MovementSystem.h"
#include "Space.h"

ska::cp::MovementSystem::MovementSystem(ska::EntityManager& em, Space& space) :
	System(em),
	m_space(space) {
}

void ska::cp::MovementSystem::refresh(unsigned int ellapsedTime) {
	const auto& entities = getEntities();
	for(const auto& entity : entities) {
		const auto& fc = m_componentAccessor.get<ska::ForceComponent>(entity);
		auto& bc = m_componentAccessor.get<ska::cp::HitboxComponent>(entity);
		auto& pc = m_componentAccessor.get<ska::PositionComponent>(entity);
		auto& body = m_space.getBodies()[bc.bodyIndex];
		const auto& shape = m_space.getShapes()[bc.shapeIndex];

		body.addForce({fc.x, fc.y});

		const auto position = body.getPosition();
		const auto& entityDimensions = shape.getDimensions();
		pc.x = static_cast<long>(position.x - entityDimensions.w / 2.F);
		pc.y = static_cast<long>(position.y - entityDimensions.h / 2.F);

		const auto rotation = body.getRotation();
		pc.rotationX = rotation.x;
		pc.rotationY = rotation.y;
	}
}
