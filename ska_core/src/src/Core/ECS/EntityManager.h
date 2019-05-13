#pragma once
#include <bitset>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <unordered_map>
#include "ECSDefines.h"
#include "ComponentTag.h"
#include "ComponentHandler.h"
#include "Base/Patterns/Observable.h"
#include "Base/Values/MovableNonCopyable.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Base/Meta/ContainsTypeTuple.h"
#include "../Data/Events/ECSEvent.h"
#include "../Data/Events/GameEventDispatcher.h"

namespace ska {

	using EntityComponentsMask = std::bitset<SKA_ECS_MAX_COMPONENTS>;
	using EntityData = std::pair<EntityComponentsMask*, EntityId>;

	enum class EntityEventTypeEnum {
		COMPONENT_ALTER
	};

	struct EntityEventType {
		EntityEventTypeEnum value;
		const EntityComponentsMask& mask;
		EntityId id;
	};

	/**
	 * \brief Manages all the component changes to an entity
	 */
	class EntityManager :
		public Observable<const EntityEventType>,
		public MovableNonCopyable {
	public:
		EntityManager(EntityManager&&) = default;
		EntityManager& operator=(EntityManager&&) = default;

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

		template <class ... ComponentType>
		static EntityManager Make(GameEventDispatcher& ged) {
			auto em = EntityManager{ ged };

			int _[] = { 0, (em.getComponents<ComponentType>() , 0)... };
			static_cast<void>(_);

			em.m_init = true;
			return em;
		}

		virtual ~EntityManager() = default;

	private:
		explicit EntityManager(GameEventDispatcher& ged) :
			m_ged(ged) { }

		EntityId createEntityNoThrow(const std::string& name = "");
		bool checkEntitiesNumber() const;

		static std::size_t& GetComponentMaskCounter();

		GameEventDispatcher& m_ged;
		std::unordered_set<EntityId> m_entities;
		std::unordered_set<EntityId> m_alteredEntities;
		EntityIdContainer m_deletedEntities;
		bool m_init = false;

		std::array<EntityComponentsMask, SKA_ECS_MAX_ENTITIES> m_componentMask;
		std::vector<std::unique_ptr<ComponentPool>> m_componentHolders;
		std::unordered_map<std::string, ComponentPool*> m_componentsNameMap {};

		void innerRemoveEntity(const EntityId& entity, ECSEvent& ecsEvent);
		void commonRemoveComponent(const EntityId& entity, ComponentPool& components);
		void commonAddComponent(const EntityId& entity, const unsigned int componentMask);

		template <class T>
		ComponentHandler<T>& getComponents() {
			if (ComponentTag<T>::id() == -1) {
				ComponentTag<T>::setId(m_componentHolders.size());
				m_componentHolders.push_back(std::make_unique<ComponentHandler<T>>(ComponentTag<T>::id(), m_componentsNameMap));
			}
			//assert((!m_init || lastMaskCounter == GetComponentMaskCounter()) && "This component doesn't belong to the declared EntityManager (when the \"Make\" factory function was called)");
			return static_cast<ComponentHandler<T>&>(*m_componentHolders[ComponentTag<T>::id()].get());
		}

	};

}
