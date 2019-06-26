#include "Core/__internalConfig/LoggerConfig.h"
#include "EntityManager.h"
#include "../Exceptions/IllegalStateException.h"
#include "../Exceptions/ExceptionTrigger.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../Data/Events/ECSEvent.h"

void ska::EntityManager::commonRemoveComponent(const EntityId& entity, ComponentPool& components) {
    const auto removedComponentMask = components.remove(entity);
    m_componentMask[entity][removedComponentMask] = false;

    m_alteredEntities.insert(entity);
}

void ska::EntityManager::refresh() {
    for(const auto& entity : m_alteredEntities) {
			auto event = EntityEventType {EntityEventTypeEnum::COMPONENT_ALTER, m_componentMask[entity], entity};
			notifyObservers(event);
    }
    m_alteredEntities.clear();
}


void ska::EntityManager::commonAddComponent(const EntityId& entity, const unsigned int componentMask) {
    m_componentMask[entity][componentMask] = true;

    m_alteredEntities.insert(entity);
}

ska::EntityId ska::EntityManager::createEntityNoThrow() {
	auto ecsEvent = ECSEvent{ ECSEventType::ENTITIES_ADDED };
	
	EntityId newId{};

	if(!checkEntitiesNumber()) {
		m_entities.erase(std::begin(m_entities));
	}

	if (m_deletedEntities.empty()) {
		newId = EntityId{m_entities.size()};
		SLOG(LogLevel::Info) << "Creating new entity id \"" << newId << "\"";
	} else {
		newId = m_deletedEntities[m_deletedEntities.size() - 1];
		SLOG(LogLevel::Info) << "Creating entity id using previously dead entity id \"" << newId << "\"";
		m_deletedEntities.pop_back();		
	}
	m_entities.insert(newId);

	/* Reset all components */
	m_componentMask[newId] &= 0;

	m_alteredEntities.insert(newId);

	ecsEvent.entities.push_back(newId);
	m_ged.ska::Observable<ECSEvent>::notifyObservers(ecsEvent);

	return newId;
}

bool ska::EntityManager::checkEntitiesNumber() const {
	return static_cast<int>(m_entities.size() - m_deletedEntities.size()) < SKA_ECS_MAX_ENTITIES;
}

ska::EntityId ska::EntityManager::createEntity() {
    if (!checkEntitiesNumber()) {
        ExceptionTrigger<ska::IllegalStateException>("Too many entities are currently in use. Unable to create a new one. "
            "Increase SKA_ECS_MAX_ENTITIES at compile time to avoid the problem.");
    }

	return createEntityNoThrow();
}

void ska::EntityManager::removeEntity(const EntityId& entity) {
	auto ecsEvent = ECSEvent{ ECSEventType::ENTITIES_REMOVED };
	innerRemoveEntity(entity, ecsEvent);
	m_ged.ska::Observable<ECSEvent>::notifyObservers(ecsEvent);
	
	/* Reset all components */
	m_componentMask[entity] &= 0;
}

void ska::EntityManager::innerRemoveEntity(const EntityId& entity, ECSEvent& ecsEvent) {
	if (m_entities.find(entity) == m_entities.end()) {
		const auto startMessage = ("Unable to delete entity #" + StringUtils::intToStr(static_cast<int>(entity)));
		SLOG(LogLevel::Error) << (startMessage + " : this entity doesn't exist or is already deleted");
		return;
	}

	m_entities.erase(entity);

	m_deletedEntities.push_back(entity);

	m_alteredEntities.insert(entity);
	
	ecsEvent.entities.push_back(entity);
}

void ska::EntityManager::removeEntities(const std::unordered_set<EntityId>& exceptions) {
	if (!m_entities.empty()) {
		auto ecsEvent = ECSEvent{ ECSEventType::ENTITIES_REMOVED };
		auto entities = m_entities;
		for (const auto& entity : entities) {
			if (exceptions.find(entity) == exceptions.end()) {
				innerRemoveEntity(entity, ecsEvent);
			}
		}
		m_ged.ska::Observable<ECSEvent>::notifyObservers(ecsEvent);

		for(auto& entity : ecsEvent.entities) {
			/* Reset all components */
			m_componentMask[entity] &= 0;
		}
	}
}

void ska::EntityManager::refreshEntity(const EntityId& entity) {
	auto event = EntityEventType {EntityEventTypeEnum::COMPONENT_ALTER, m_componentMask[entity], entity};
	notifyObservers(event);
}

void ska::EntityManager::refreshEntities() {
	for (const auto& entity : m_entities) {
		auto event = EntityEventType {EntityEventTypeEnum::COMPONENT_ALTER, m_componentMask[entity], entity};
		notifyObservers(event);
	}
}
