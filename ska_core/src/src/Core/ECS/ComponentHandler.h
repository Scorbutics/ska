#pragma once
#include <vector>
#include <typeinfo>
#include <optional>
#include <cassert>
#include "ECSDefines.h"
#include "SerializeComponent.h"
#include "ComponentPool.h"
#include "Base/Config/LoggerConfig.h"

namespace ska {
	/**
	 * \brief Holds an array of a specific component type and his associated mask.
	 * \tparam T The component type held
	 */
	template <typename T>
	class ComponentHandler :
        public ComponentPool {

	public:
		ComponentHandler(unsigned int mask, std::unordered_map<std::string, ComponentPool*>& mapComponentNames):
			m_mask(mask) {
			//TODO
            //SLOG(LogLevel::Debug) << "Initializing component with mask " <<  m_mask;
			m_entitiesWithComponent.resize(SKA_ECS_MAX_ENTITIES);
			
			if constexpr(has_getClassName<T>::value) {
				mapComponentNames.emplace(T::getClassName(), this);
			} else {
				mapComponentNames.emplace("", this);
			}
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

		T& getComponent(const EntityId& id) {
			assert(id < m_entitiesWithComponent.size() && m_entitiesWithComponent[id].has_value() && "This entity has no component with this id");
			auto& componentIdForEntity = m_entitiesWithComponent[id].value();
			assert(componentIdForEntity < m_components.size() && "Invalid component for this entity");
			return m_components[componentIdForEntity];
		}

		unsigned int getMask() const {
			return m_mask;
		}

		std::string serialize(const EntityId& entityId, const std::string& field) override {
			if constexpr(has_getClassName<T>::value) {
				return ska::SerializeComponent<T>::serialize(getComponent(entityId), field);
			}

			assert(has_getClassName<T>::value && ("This component should be serializable in order to be serialized (field required : " + field + ")").c_str());
			return "";	
		}

		void deserialize(const EntityId& entityId, const std::string& field, const std::string& value) override {
			if constexpr(has_getClassName<T>::value) {
				ska::SerializeComponent<T>::deserialize(getComponent(entityId), field, value);
			} else {
				assert(has_getClassName<T>::value && ("This component should be serializable in order to be deserialized (field required : " + field + ")").c_str());
			}
		}

		virtual ~ComponentHandler() = default;

	private:
		std::vector<T> m_components;
		std::vector<std::optional<ComponentId>> m_entitiesWithComponent;
		const unsigned int m_mask;
	};


}
