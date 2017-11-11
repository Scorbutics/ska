#pragma once
#include <unordered_set>
#include "SystemMaskGenerator.h"
#include "ComponentSafeAccessor.h"
#include "ISystem.h"
#include "../Utils/Observer.h"
#include "../Utils/Refreshable.h"
#include "ComponentUnsafeAccessor.h"

namespace ska {
    template <class ... ComponentType>
    struct RequiredComponent;

    template <class ... ComponentType>
    struct PossibleComponent;

	template <class Storage, class RComponentType, class PComponentType>
	class System;

	/**
     * \brief Base class of a system, where system is defined as the "S" of "Entity Component System" (ECS).
     * \tparam Storage A storage class that indicates the way entities must be stored in the system
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
    template <class Storage, class ... RComponentType, class ... PComponentType>
    class System <Storage, RequiredComponent<RComponentType...>, PossibleComponent<PComponentType...>> :
        public Observer<const EntityEventType, const EntityComponentsMask&, EntityId>,
        virtual public ISystem,
        virtual protected Refreshable {

    protected:
        using SystemType = System <Storage, RequiredComponent<RComponentType...>, PossibleComponent<PComponentType...>>;
        using MaskGenerator = SystemMaskGenerator<RComponentType...>;

    public :
        explicit System(EntityManager& entityManager) :
            Observer(std::bind(&System::onComponentModified, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
            m_entityManager(entityManager),
            m_componentAccessor(entityManager),
    		m_componentPossibleAccessor(entityManager) {
            m_entityManager.addObserver(*this);

            MaskGenerator smg(m_entityManager);
            smg.generate(m_systemComponentMask);
        }

        void operator=(const SystemType& sys) = delete;

	    virtual void update(unsigned int ellapsedTime) override {
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

        virtual ~System(){ m_entityManager.removeObserver(*this); }

    private:
        EntityManager& m_entityManager;
        std::unordered_set<EntityId> m_toDelete;
        EntityComponentsMask m_systemComponentMask;
        Storage m_processed;

        bool onComponentModified(const EntityEventType& e, const EntityComponentsMask& mask, EntityId entityId) {

            /* An entity belongs to the system ONLY IF it has ALL the requiered components of the system */
            auto resultMask = mask & m_systemComponentMask;

            switch (e) {
            case COMPONENT_ADD:
                if (resultMask == m_systemComponentMask && m_processed.count(entityId) == 0) {
                    m_processed.insert(entityId);
                }
                break;

            case COMPONENT_REMOVE:
                if (resultMask != m_systemComponentMask && m_processed.count(entityId) > 0) {
                    m_processed.erase(entityId);
                }
                break;

            case COMPONENT_ALTER:
                if(resultMask == m_systemComponentMask) {
                    m_processed.insert(entityId);
                } else {
                    m_processed.erase(entityId);
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

        const Storage& getEntities() {
            return m_processed;
        }

		EntityId createEntity() const{
			return m_entityManager.createEntity();
		}

		EntityId createEntityNoThrow() const {
			return m_entityManager.createEntityNoThrow();
		}

		void removeEntity(EntityId entity) const {
			m_entityManager.removeEntity(entity);
        }

		void refreshEntity(EntityId entity) const {
			m_entityManager.refreshEntity(entity);
        }
    };

}
