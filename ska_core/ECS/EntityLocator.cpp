#include "../Logging/LoggerRTTI.h"
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
	if (ecsEvent.ecsEventType == ECSEventType::ENTITIES_REMOVED) {
		for (const auto& entityEntry : ecsEvent.entities) {
			m_entitiesReverseNameMap[entityEntry.first] = "";
			m_entitiesNameMap.erase(entityEntry.second);
		}
	} else {
		for (const auto& entityEntry : ecsEvent.entities) {
			m_entitiesReverseNameMap[entityEntry.first] = entityEntry.second;
			m_entitiesNameMap[entityEntry.second] = entityEntry.first;
		}
	}
	return true;
}
