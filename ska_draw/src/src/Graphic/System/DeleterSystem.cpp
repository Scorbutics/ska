#include "DeleterSystem.h"
#include "Core/Utils/TimeUtils.h"

ska::DeleterSystem::DeleterSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::DeleterSystem::refresh(unsigned int) {
	m_toDelete.clear();
	const auto& processed = getEntities();
	for (const auto& entityId : processed) {
		auto& dc = m_componentAccessor.get<DeleterComponent>(entityId);
		const int elapsed = TimeUtils::getTicks() - dc.startTime;
		if (elapsed >= dc.delay) {
			m_toDelete.emplace(entityId);
		}
	}

	for (const auto& entityId : m_toDelete) {
		removeEntity(entityId);
	}
}

bool ska::DeleterSystem::wasLoopDeletedEntities(ska::EntityId& entity) const{
	return m_toDelete.count(entity) > 0;
}

ska::DeleterSystem::~DeleterSystem() {
}
