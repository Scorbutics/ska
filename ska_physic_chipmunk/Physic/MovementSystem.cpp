#include "MovementSystem.h"
#include "Vect.h"
#include "Space.h"

ska::cp::MovementSystem::MovementSystem(ska::EntityManager& em, Space& space) :
	System(em),
	m_space(space) {
}

void ska::cp::MovementSystem::refresh(unsigned int ellapsedTime) {
	const auto& entities = getEntities();
	for(const auto& entity : entities) {
		const auto& fc = m_componentAccessor.get<ska::ForceComponent>(entity);
		auto& bc = m_componentAccessor.get<ska::cp::BodyComponent>(entity);
		auto& pc = m_componentAccessor.get<ska::PositionComponent>(entity);
		auto& body = m_space.getBodies()[bc.bodyIndex];

		body.setVelocity({fc.x, fc.y});

		pc.x = body.getPosition().x;
		pc.y = body.getPosition().y;
	}
}
