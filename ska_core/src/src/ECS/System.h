#pragma once
#include <unordered_set>
#include "SystemMaskGenerator.h"
#include "ComponentSafeAccessor.h"
#include "ISystem.h"
#include "../Utils/SubObserver.h"
#include "../Utils/Refreshable.h"
#include "ComponentUnsafeAccessor.h"

namespace ska {
    template <class ... ComponentType>
    struct RequiredComponent;

    template <class ... ComponentType>
    struct PossibleComponent;

	template <class RComponentType, class PComponentType>
	class System;

	/**
     * \brief Base class of a system, where system is defined as the "S" of "Entity Component System" (ECS).
     * \tparam RComponentType List of the required component types
     * \tparam PComponentType List of the possible component types
     * 
     * Systems contain the logic part of an ECS. They act on a group of component (or entities), access to data and alter it.
     * This class is template parametrized in order to describe which component will be mandatory, and which ones might be present.
     * On top of that, every component access is generally done by using a ComponentSafeAccessor or a ComponentUnsafeAccessor, 
     * that makes the user avoiding mistakes by accessing a component that is not managed by the system.
     * 
     * Entities are automatically added / removed to the internal Storage of the system depending on which (required) component they have.
     * Possible components do not influate on this.
     */
    template <class ... RComponentType, class ... PComponentType>
    class System <RequiredComponent<RComponentType...>, PossibleComponent<PComponentType...>> :
        public SubObserver<const EntityEventType>,
        virtual public ISystem,
        virtual protected Refreshable {

    protected:
        using SystemType = System <RequiredComponent<RComponentType...>, PossibleComponent<PComponentType...>>;
        using MaskGenerator = SystemMaskGenerator<RComponentType...>;

    public :
        explicit System(EntityManager& entityManager) :
			SubObserver(std::bind(&System::onComponentModified, this, std::placeholders::_1), entityManager),
            m_entityManager(entityManager),
            m_componentAccessor(entityManager),
    		m_componentPossibleAccessor(entityManager) {
            
			auto smg = MaskGenerator{ m_entityManager };
            smg.generate(m_systemComponentMask);
        }

        void operator=(const SystemType& sys) = delete;

	    void update(unsigned int ellapsedTime) override {
            refresh(ellapsedTime);
            if (!m_toDelete.empty()) {
                for (auto entity : m_toDelete) {
                    m_entityManager.removeEntity(entity);
                }
                m_toDelete.clear();
            }
        }

        void scheduleDeferredRemove(EntityId e) {
            m_toDelete.emplace(e);
        }

		virtual ~System() = default;

    private:
        EntityManager& m_entityManager;
        std::unordered_set<EntityId> m_toDelete;
		std::unordered_set<EntityId> m_processed;
		EntityComponentsMask m_systemComponentMask;

        bool onComponentModified(const EntityEventType& event) {

            /* An entity belongs to the system ONLY IF it has ALL the requiered components of the system */
	        const auto resultMask = event.mask & m_systemComponentMask;

            switch (event.value) {
            case EntityEventTypeEnum::COMPONENT_ALTER:
                if(resultMask == m_systemComponentMask) {
                    m_processed.insert(event.id);
                } else {
                    m_processed.erase(event.id);
                }
                break;

            default:
                break;
            }
            return true;
        }

    protected:
        using ComponentAccessor = ComponentSafeAccessor<RComponentType...>;
		using ComponentPossibleAccessor = ComponentUnsafeAccessor<PComponentType...>;
        ComponentAccessor m_componentAccessor;
		ComponentPossibleAccessor m_componentPossibleAccessor;

        const std::unordered_set<EntityId>& getEntities() const {
            return m_processed;
        }

		EntityId createEntity() const {
			return m_entityManager.createEntity();
		}

		EntityId createEntityNoThrow() const {
			return m_entityManager.createEntityNoThrow();
		}

		void removeEntity(const EntityId& entity) const {
			m_entityManager.removeEntity(entity);
        }

		void refreshEntity(const EntityId& entity) const {
			m_entityManager.refreshEntity(entity);
        }
    };

}
