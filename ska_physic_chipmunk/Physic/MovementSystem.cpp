#include "MovementSystem.h"
#include "Vect.h"

ska::cp::MovementSystem::MovementSystem(ska::EntityManager& em) :
	System(em) {
}

void ska::cp::MovementSystem::refresh(unsigned int ellapsedTime) {
	auto& entities = getEntities();
	for(const auto& entity : entities) {
		const auto& fc = m_componentAccessor.get<ska::ForceComponent>(entity);
		auto& bc = m_componentAccessor.get<ska::cp::BodyComponent>(entity);
		auto& body = bc.space->getBodies()[bc.bodyIndex];

		body.setVelocity({fc.x, fc.y});
	}
}
