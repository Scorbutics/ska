#include "__internalConfig/LoggerConfig.h"
#include "EntityHandles.h"
#include <stdexcept>
#include <string>

ska::scripting::EntityHandles::EntityHandles(GameEventDispatcher& ged) :
	SubObserver(std::bind(&EntityHandles::onECSEvent, this, std::placeholders::_1), ged) {
}

long ska::scripting::EntityHandles::handle(EntityId entity) const {
	if (entity >= m_generations.size()) {
		throw std::runtime_error("invalid entity id " + std::to_string(entity));
	}
	return static_cast<long>(m_generations[entity]) * IdRange + static_cast<long>(entity);
}

long ska::scripting::EntityHandles::handle(const std::optional<EntityId>& entity) const {
	return entity.has_value() ? handle(*entity) : -1L;
}

std::optional<ska::EntityId> ska::scripting::EntityHandles::find(long handle) const {
	if (handle < 0) {
		return {};
	}
	const auto entity = static_cast<EntityId>(handle % IdRange);
	const auto generation = handle / IdRange;
	if (entity >= m_generations.size() || generation != static_cast<long>(m_generations[entity])) {
		return {};
	}
	return entity;
}

ska::EntityId ska::scripting::EntityHandles::entity(long handle) const {
	if (handle < 0) {
		throw std::runtime_error("no entity (" + std::to_string(handle) + ")");
	}
	const auto entity = find(handle);
	if (!entity.has_value()) {
		throw std::runtime_error("the entity " + std::to_string(handle) + " does not exist anymore (removed)");
	}
	return *entity;
}

bool ska::scripting::EntityHandles::onECSEvent(ECSEvent& ecsEvent) {
	if (ecsEvent.ecsEventType == ECSEventType::ENTITIES_REMOVED) {
		for (const auto entity : ecsEvent.entities) {
			if (entity < m_generations.size()) {
				m_generations[entity] = static_cast<std::uint16_t>((m_generations[entity] + 1) % GenerationRange);
			}
		}
	}
	return true;
}
