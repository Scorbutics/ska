#pragma once
#include "EntityManager.h"
#include "../Exceptions/IllegalStateException.h"
#include "../Exceptions/ExceptionTrigger.h"

namespace ska {
    template <class ... ComponentType>
    class SystemMaskGenerator {
    public:
	    explicit SystemMaskGenerator(EntityManager& entityManager) : 
    		m_entityManager(entityManager) {
        }
        ~SystemMaskGenerator() = default;

	    /**
         * \brief Retrieves all the components masks using the variadic template with ComponentType
		 *	   We "iterate" through each ComponentType with the bracket initializer trick and
		 *	   we add each component mask to the system component mask with a binary OR.
         * \param ecm Entity component mask
         */
        void generate(EntityComponentsMask& ecm) {
		SKA_LOG_DEBUG("Initializing system with components :");
        	int _[] = { 0, (buildSystemMask<ComponentType>(ecm) , 0)... };
		(void)_;
		SKA_LOG_DEBUG("End system initialization\n\n");;
        }

    private:
        template <class T>
		void buildSystemMask(EntityComponentsMask& systemComponentMask) {
			unsigned int mask = m_entityManager.template getMask<T>();
			if (mask >= systemComponentMask.size()) {
				ExceptionTrigger<IllegalStateException>("Too many components are used in the game. Unable to continue.", ExceptionAbort);
			}
			SKA_LOG_DEBUG("\t - mask ", mask);

			systemComponentMask[mask] = true;
		}

		EntityManager& m_entityManager;
    };
}
