#include "MovementSystem.h"
#include "Space.h"
#include "ECS/Basics/Physic/MovementComponent.h"

ska::cp::MovementSystem::MovementSystem(ska::EntityManager& em, Space& space) :
	System(em),
	m_space(space) {
}

void ska::cp::MovementSystem::adjustVelocity(const ska::EntityId& entityId, ska::cp::Body& body, const ska::ForceComponent& fc) {
	auto bodyVelocity = ska::Point<float>{ fc.x * 10 , fc.y * 10 };
	body.setVelocity(std::move(bodyVelocity));
}

void ska::cp::MovementSystem::refresh(unsigned int ellapsedTime) {
	const auto& entities = getEntities();
	for (const auto& entity : entities) {
		auto& fc = m_componentAccessor.get<ska::ForceComponent>(entity);
		const auto& bc = m_componentAccessor.get<ska::cp::HitboxComponent>(entity);
		auto& pc = m_componentAccessor.get<ska::PositionComponent>(entity);
		auto& mc = m_componentAccessor.get<ska::MovementComponent>(entity);

		auto& controlBody = m_space.getBody(bc.controlBodyIndex);

		//Velocity : control body
		if (bc.controlBodyIndex != bc.bodyIndex) {
			adjustVelocity(entity, controlBody, fc);
		}

		mc.vx = controlBody.getVelocity().x / 50;
		mc.vy = controlBody.getVelocity().y / 50;
		fc.z = fc.y = fc.x = 0;
		
		const auto& body = m_space.getBody(bc.bodyIndex);

		//Position : body
		const auto position = body.getPosition();
		const auto& shape = m_space.getShape(bc.shapeIndex);
		const auto& entityDimensions = shape.getDimensions();
		pc.x = position.x;
		pc.y = position.y;

		//Rotation : body
		const auto rotation = body.getRotation();
		pc.rotationX = rotation.x;
		pc.rotationY = rotation.y;

		//Update control body to keep it always near the real body
		controlBody.setPosition(position);
	}
}
