#include "GravitySystem.h"

static constexpr auto EARTH_GRAVITY = 9.81F;

ska::GravitySystem::GravitySystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::GravitySystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& forceComponent = m_componentAccessor.get<ForceComponent>(entityId);
		auto& gaComponent = m_componentAccessor.get<GravityAffectedComponent>(entityId);
		auto& moveComponent = m_componentAccessor.get<MovementComponent>(entityId);

		//PFD selon x et y avec la force de frottements fluides
		//( en comptant qu'au préalable on a peut-être déjà appliqué une ou plusieurs force(s) )
		forceComponent.x -= gaComponent.friction * (moveComponent.vx + moveComponent.ax);
		forceComponent.y -= gaComponent.friction * (moveComponent.vy + moveComponent.ay);

		//EARTH_GRAVITY trop élevée
		forceComponent.z -= (EARTH_GRAVITY / 10.0F) * forceComponent.weight;
	}
}

ska::GravitySystem::~GravitySystem()
{
}
