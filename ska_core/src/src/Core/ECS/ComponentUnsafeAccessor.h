#pragma once
#include "ECSDefines.h"
#include "EntityManager.h"
#include "Base/Meta/ContainsTypeTuple.h"

namespace ska {
	/**
	* \brief Provides a statically type check when accessing to a component of the entity manager.
	* In contrary to ComponentSafeAccessor that simply disallow access, it returns a null pointer when the entity has no component.
	* \tparam ComponentType The component type list to access through this class
	*/
    template <class ... ComponentType>
    class ComponentUnsafeAccessor {
    public:
        ComponentUnsafeAccessor(EntityManager& entityManager) :
            m_entityManager(entityManager) {
        }
        ~ComponentUnsafeAccessor() = default;

        template <class T>
        T* get(EntityId entityId) const {
            using componentTypeFound = meta::contains<T, ComponentType...>;
            static_assert(componentTypeFound::value, "Unable to use this component : it doesn't belong to the current system");
			if(m_entityManager.hasComponent<T>(entityId)) {
				return &m_entityManager.getComponent<T>(entityId);
			}
			return nullptr;
		}
    private:
        EntityManager& m_entityManager;
    };
}
