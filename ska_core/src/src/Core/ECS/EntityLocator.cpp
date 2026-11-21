#include "Core/__internalConfig/LoggerConfig.h"
#include "EntityLocator.h"

ska::EntityLocator::EntityLocator(GameEventDispatcher& ged) :
	SubObserver(std::bind(&EntityLocator::onECSEvent, this, std::placeholders::_1), ged) {
}

const ska::EntityId* ska::EntityLocator::getEntityId(const std::string& name) const {
	if (m_entitiesNameMap.find(name) == m_entitiesNameMap.end()) {
		return nullptr;
	}
	return &m_entitiesNameMap.at(name);
}

bool ska::EntityLocator::onECSEvent(ECSEvent& ecsEvent) {
	switch (ecsEvent.ecsEventType) {
	case ECSEventType::ENTITY_NAMED:
		for (auto index = std::size_t{ 0 }; index < ecsEvent.entities.size() && index < ecsEvent.names.size(); index++) {
			const auto entity = ecsEvent.entities[index];
			if (entity >= m_entitiesReverseNameMap.size()) {
				continue;
			}
			forget(entity);
			m_entitiesReverseNameMap[entity] = ecsEvent.names[index];
			if (!ecsEvent.names[index].empty()) {
				m_entitiesNameMap[ecsEvent.names[index]] = entity;
			}
		}
		break;
	case ECSEventType::ENTITIES_REMOVED:
		for (const auto entity : ecsEvent.entities) {
			forget(entity);
		}
		break;
	default:
		break;
	}
	return true;
}

void ska::EntityLocator::forget(const EntityId entity) {
	if (entity >= m_entitiesReverseNameMap.size()) {
		return;
	}
	auto& name = m_entitiesReverseNameMap[entity];
	if (!name.empty()) {
		const auto named = m_entitiesNameMap.find(name);
		if (named != m_entitiesNameMap.end() && named->second == entity) {
			m_entitiesNameMap.erase(named);
		}
		name.clear();
	}
}
