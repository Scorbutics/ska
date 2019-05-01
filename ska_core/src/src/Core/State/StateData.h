#pragma once

namespace ska {
	
	/**
	 * \brief Group of data that is transmitted to every game state
	 * \tparam EM Type of the holded entity manager
	 * \tparam ED Type of the holded event dispatcher
	 */
	template<class EM, class ED>
	struct StateData {
		StateData(EM& em, ED& ed) :
			m_entityManager(em),
			m_eventDispatcher(ed) {
		}
		EM& m_entityManager;
		ED& m_eventDispatcher;
	};
}
