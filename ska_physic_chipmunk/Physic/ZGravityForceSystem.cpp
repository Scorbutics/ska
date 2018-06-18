#include "ZGravityForceSystem.h"
#include "Space.h"

ska::ZGravityForceSystem::ZGravityForceSystem(ska::EntityManager& em, const float gravityPower) :
	System(em),
	m_gravityPower(gravityPower) {
}

void ska::ZGravityForceSystem::refresh(unsigned int ellapsedTime) {
	const auto& entities = getEntities();
	for (const auto& entity : entities) {
		auto& fc = m_componentAccessor.get<ska::ForceComponent>(entity);
		fc.z = - m_gravityPower;
	}
}
