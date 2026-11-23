#pragma once
#include <memory>
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
		// Names an entity : it can then be found by name (EntityLocator)
		void setName(const EntityId& entity, const std::string& name);
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

		// Names of the component types of an entity (for the tools : the development console)
		std::vector<std::string> componentNames(EntityId entity) const {
			auto names = std::vector<std::string>{};
			if (m_entities.find(entity) == m_entities.end()) {
				return names;
			}
			for (const auto& holder : m_componentHolders) {
				if (holder == nullptr) {
					continue;
				}
				auto& handler = *holder;
				// The mask bit of a component type is its type id : its index in m_componentHolders
				const auto index = static_cast<std::size_t>(&holder - &m_componentHolders[0]);
				if (index < SKA_ECS_MAX_COMPONENTS && m_componentMask[entity][index]) {
					names.push_back(handler.typeName());
				}
			}
			return names;
		}
		// Every entity alive
		const std::unordered_set<EntityId>& entities() const { return m_entities; }

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
			} else if (m_componentHolders.size() <= Component<T>::TYPE_ID() || m_componentHolders[Component<T>::TYPE_ID()] == nullptr) {
				// The type ids are global : another entity manager can have created this type (its storage is created here)
				if (m_componentHolders.size() <= Component<T>::TYPE_ID()) {
					m_componentHolders.resize(static_cast<const std::size_t>(Component<T>::TYPE_ID() * 1.5 + 1));
				}
				m_componentHolders[Component<T>::TYPE_ID()] = std::make_unique<ComponentHandler<T>>(Component<T>::TYPE_ID());
			}
			
			return static_cast<ComponentHandler<T>&>(*m_componentHolders[Component<T>::TYPE_ID()].get());
		}

	};

}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::EntityManager)