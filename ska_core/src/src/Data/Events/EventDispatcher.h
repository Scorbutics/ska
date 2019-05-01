#pragma once

#include "../../Utils/Observable.h"

namespace ska {

	/**
	 * \brief Handles event transmission between transmitters and receivers.
	 * \tparam ET The list of event types handled by this event dispatcher
	 */
	template <class ...ET>
	class EventDispatcher : public Observable<ET>... {
	public:
		EventDispatcher() = default;
		EventDispatcher(const EventDispatcher<ET...>&) = delete;
		EventDispatcher(EventDispatcher&&) = default;

		/**
		 * \brief Adds several observers simultaneously
		 * \tparam ET2 The list of event types handled by observers "obs"
		 * \param obs The observer list to be added
		 */
		template <class ...ET2>
		void addMultipleObservers(Observer<ET2>&... obs) {
			int _[] = { 0, (Observable<ET2>::addObserver(obs), 0)... };
			(void)_;
		}

		/**
		* \brief Removes several observers simultaneously
		* \tparam ET2 The list of event types handled by observers "obs"
		* \param obs The observer list to be removed
		*/
		template <class ...ET2>
		void removeMultipleObservers(Observer<ET2>&... obs) {
			int _[] = { 0, (Observable<ET2>::removeObserver(obs), 0)... };
			(void)_;
		}

		virtual ~EventDispatcher() = default;
	};

}
