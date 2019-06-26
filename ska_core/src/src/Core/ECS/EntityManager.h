#pragma once
#include <bitset>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <unordered_map>
#include "Component.h"
#include "ECSDefines.h"
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
		explicit EntityManager(GameEventDispatcher& ged) :
			m_ged(ged) { }
		virtual ~EntityManager() = default;

		EntityManager(EntityManager&&) = default;
		EntityManager& operator=(EntityManager&&) = default;

		EntityId createEntity();

		void removeEntity(const EntityId& entity);
		void removeEntities(const std::unordered_set<EntityId>& exceptions = std::unordered_set<ska::EntityId>{});
		void refreshEntity(const EntityId& entity);
		void refreshEntities();
		void refresh();

		template <class T>
		void addComponent(EntityId entity, T&& component) {
			ComponentHandler<T>& components = this->template getComponents<T>();
			SLOG(LogLevel::Info) << "Adding component \"" << Component<T>::TYPE_NAME() << "\" for entity id \"" << entity << "\"";
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
			SLOG(LogLevel::Info) << "Removing component \"" << Component<T>::TYPE_NAME() << "\" for entity id \"" << entity << "\"";
			commonRemoveComponent(entity, components);
		}

		template <class T>
		unsigned int getMask() {
			ComponentHandler<T>& components = this->template getComponents<T>();
			return components.getMask();
		}

	private:
		EntityId createEntityNoThrow();
		bool checkEntitiesNumber() const;

		GameEventDispatcher& m_ged;
		std::unordered_set<EntityId> m_entities;
		std::unordered_set<EntityId> m_alteredEntities;
		EntityIdContainer m_deletedEntities;

		std::array<EntityComponentsMask, SKA_ECS_MAX_ENTITIES> m_componentMask;
		std::vector<std::unique_ptr<ComponentPool>> m_componentHolders;

		void innerRemoveEntity(const EntityId& entity, ECSEvent& ecsEvent);
		void commonRemoveComponent(const EntityId& entity, ComponentPool& components);
		void commonAddComponent(const EntityId& entity, const unsigned int componentMask);

		template <class T>
		ComponentHandler<T>& getComponents() {
			if (Component<T>::TYPE_ID() == -1) {
				Component<T>::TYPE_ID() = m_componentHolders.size();
				m_componentHolders.push_back(std::make_unique<ComponentHandler<T>>(Component<T>::TYPE_ID()));
			} else if (m_componentHolders.size() <= Component<T>::TYPE_ID()) {
				m_componentHolders.resize(static_cast<const std::size_t>(Component<T>::TYPE_ID() * 1.5 + 1));
				m_componentHolders[Component<T>::TYPE_ID()] = std::make_unique<ComponentHandler<T>>(Component<T>::TYPE_ID());
			}
			
			return static_cast<ComponentHandler<T>&>(*m_componentHolders[Component<T>::TYPE_ID()].get());
		}

	};

}
