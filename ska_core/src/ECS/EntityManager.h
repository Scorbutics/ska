#pragma once
#include <bitset>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <unordered_map>
#include "ECSDefines.h"
#include "ComponentHandler.h"
#include "../Utils/Observable.h"
#include "../Utils/StringUtils.h"
#include "../Data/Events/ECSEvent.h"
#include "../Data/Events/GameEventDispatcher.h"

namespace ska {

	using EntityComponentsMask = std::bitset<SKA_ECS_MAX_COMPONENTS>;
	using EntityData = std::pair<EntityComponentsMask*, EntityId>;

	enum class EntityEventType {
		COMPONENT_ALTER
	};

	/**
	 * \brief Manages all the component changes to an entity
	 */
	class EntityManager :
	    public Observable<const EntityEventType, const EntityComponentsMask&, EntityId> {
	public:
		explicit EntityManager(GameEventDispatcher& ged) : 
			m_ged(ged) { }

		EntityId createEntity(const std::string& name = "");

		void removeEntity(const EntityId& entity);
		void removeEntities(const std::unordered_set<EntityId>& exceptions = std::unordered_set<ska::EntityId>{});
		void refreshEntity(const EntityId& entity);
		void refreshEntities();
        void refresh();

		std::string serializeComponent(const EntityId& entityId, const std::string& component, const std::string& field) const;
		void deserializeComponent(const EntityId& entityId, const std::string& component, const std::string& field, const std::string& value);

		void removeComponent(const EntityId& entity, const std::string& component);
		void addComponent(const EntityId& entity, const std::string& component);

		template <class T>
		void addComponent(EntityId entity, T&& component) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			commonAddComponent(entity, components.add(entity, std::forward<T>(component)));
		}

		template <class T>
		T& getComponent(EntityId entityId) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			T& result = components.getComponent(entityId);
			return result;
		}

		template <class T>
		bool hasComponent(EntityId entityId) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return m_componentMask[entityId][components.getMask()];
		}

		template <class T>
		void removeComponent(EntityId entity) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			commonRemoveComponent(entity, components);
		}

		template <class T>
		unsigned int getMask() {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return components.getMask();
		}

		virtual ~EntityManager() = default;

	private:
		EntityId createEntityNoThrow(const std::string& name = "");
		bool checkEntitiesNumber() const;

		GameEventDispatcher& m_ged;
		std::array<EntityComponentsMask, SKA_ECS_MAX_ENTITIES> m_componentMask;
		std::unordered_set<EntityId> m_entities;
		std::unordered_set<EntityId> m_alteredEntities;
		EntityIdContainer m_deletedEntities;
		static unsigned int m_componentMaskCounter;

		std::unordered_map<std::string, ComponentPool*> m_componentsNameMap;

		void innerRemoveEntity(const EntityId& entity, ECSEvent& ecsEvent);

		template <class T>
		ComponentHandler<T>& getComponents() {
			static ComponentHandler<T> components(m_componentMaskCounter++, m_componentsNameMap);
			return components;
		}

		void commonRemoveComponent(const EntityId& entity, ComponentPool& components);
		void commonAddComponent(const EntityId& entity, const unsigned int componentMask);
        
	};

}