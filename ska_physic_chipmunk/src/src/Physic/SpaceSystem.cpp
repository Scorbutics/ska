#include "SpaceSystem.h"
#include "Logging/Logger.h"

ska::cp::SpaceSystem::SpaceSystem(EntityManager& em) :
	System(em) {
}

ska::cp::Space& ska::cp::SpaceSystem::getSpace() {
	return m_space;
}

void ska::cp::SpaceSystem::reset() {
	m_space = std::move(ska::cp::Space{});
}

void ska::cp::SpaceSystem::refresh(unsigned int ellapsedTime) {
	const auto& entities = getEntities();
	for (const auto& entity : entities) {
		const auto& bc = m_componentAccessor.get<ska::cp::HitboxComponent>(entity);
		auto& pc = m_componentAccessor.get<ska::PositionComponent>(entity);
		auto& mc = m_componentAccessor.get<ska::MovementComponent>(entity);
		auto& fc = m_componentAccessor.get<ska::ForceComponent>(entity);

		//linear Z variation handled manually (fake 3D)
		pc.z += mc.vz;
		mc.vz += fc.z;

		if (pc.z < 0) {
			pc.z = 0;
			mc.vz = 0;
		}
		mc.vz -= FLUID_Z_FRICTION_FORCE * mc.vz;
		fc.z = 0;

		auto& body = m_space.getBody(bc.bodyIndex);
		const auto position = body.getPosition();

		//Handles "teleportation"
		if (pc.x != static_cast<long>(position.x)
			|| pc.y != static_cast<long>(position.y)) {
			SLOG(ska::LogLevel::Info) << "Teleporting entity " << entity << " to location " << pc.x << "; " << pc.y;
			body.setPosition({ pc.x, pc.y });
		}
	}
	m_space.step(ellapsedTime / 1000.);
}


