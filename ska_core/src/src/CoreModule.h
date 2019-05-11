#pragma once
#include <string>
#include "Core/Inputs/EnumContextManager.h"
#include "Core/Inputs/InputContextManager.h"

namespace ska {
    template <class EntityManager>
	class CoreModule {
	public:
		CoreModule(const std::string& moduleName, GameEventDispatcher& ged):
    		m_entityManager(ged),
            m_playerICM(m_rawInputListener, ged),
            m_name(moduleName) {
		}

		void eventUpdate(unsigned int ellapsedTime) {
            m_entityManager.refresh();

            /* Raw input acquisition */
            m_playerICM.refresh();
		}

        /**
         * \brief Adds an input context to the game.
         * \tparam I input context type
         * \tparam Args additional parameters to the input context constructor
         * \param em EnumContextManager
         * \param args Args&&... additional parameters to the input context constructor
         *
         * Having multiple input contexts in a game allow to have specific ways to query inputs depending from where we are in the game.
         */
        template<class I, class ... Args>
        void addInputContext(EnumContextManager em, Args&&... args) {
            m_playerICM.addContext(em, std::make_unique<I>(m_rawInputListener, std::forward<Args>(args)...));
        }

		~CoreModule() = default;

		EntityManager& getEntityManager() {
            return m_entityManager;
		}

    private:
        RawInputListener m_rawInputListener;
        EntityManager m_entityManager;
        InputContextManager m_playerICM;
		std::string m_name;
	};
}
