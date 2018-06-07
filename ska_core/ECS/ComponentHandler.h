#pragma once
#include <vector>
#include <typeinfo>
#include <optional>
#include <cassert>
#include "ECSDefines.h"
#include "SerializeComponent.h"
#include "ComponentSerializer.h"
#include "../Logging/Logger.h"

namespace ska {
	/**
	 * \brief Holds an array of a specific component type and his associated mask.
	 * \tparam T The component type held
	 */
	template <typename T>
	class ComponentHandler :
        public ComponentSerializer {

	public:
		ComponentHandler(unsigned int mask, std::unordered_map<std::string, ComponentSerializer*>& mapComponentNames):
			m_mask(mask) {
			SKA_LOG_DEBUG("Initializing component type ", "", " with mask ", m_mask);
			m_entitiesWithComponent.resize(SKA_ECS_MAX_ENTITIES);
			mapComponentNames.emplace("", this);
		}

		unsigned int addEmpty(const EntityId&) override {
			/* We can add operations here depending of the ComponentHandler implementation */
			return m_mask;
		}

		unsigned int add(const EntityId& entityId, T&& comp) {
			const auto componentIdForEntity = m_entitiesWithComponent[entityId];
			if(componentIdForEntity.has_value() && componentIdForEntity.value() < m_components.size()) {
				m_components[componentIdForEntity.value()] = std::forward<T>(comp);
			} else {
				m_components.push_back(std::forward<T>(comp));
				m_entitiesWithComponent[entityId] = m_components.size() - 1;
			}
			return m_mask;
		}

		unsigned int remove(const EntityId& entityId) override {
			if(m_entitiesWithComponent[entityId].has_value()) {
				const auto componentIdForEntity = m_entitiesWithComponent[entityId].value();
				m_components[componentIdForEntity] = T();
			}
			return m_mask;
		}

		std::string getComponentField(const EntityId& id, const std::string& field) override {
			return ska::SerializeComponent<T>::serialize(getComponent(id), field);
		}

		T& getComponent(const EntityId& id) {
			assert(id < m_entitiesWithComponent.size() && m_entitiesWithComponent[id].has_value() && "This entity has no component with this id");
			auto& componentIdForEntity = m_entitiesWithComponent[id].value();
			assert(componentIdForEntity < m_components.size() && "Invalid component for this entity");
			return m_components[componentIdForEntity];
		}

		unsigned int getMask() const {
			return m_mask;
		}

		virtual ~ComponentHandler() = default;

	private:
		std::vector<T> m_components;
		std::vector<std::optional<ComponentId>> m_entitiesWithComponent;
		const unsigned int m_mask;
	};


}
