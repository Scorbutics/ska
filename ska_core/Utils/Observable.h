#pragma once

#include <vector>
#include <algorithm>
#include "Observer.h"
#include "../Logging/Logger.h"

namespace ska {

	template <class T, class ...Args>
	class Observable {
		using ObserverType = Observer<T, Args...>;
	public:
		Observable() = default;

		void addObserver(ObserverType& obs) {
			m_head.push_back(&obs);
		}

		void removeObserver(ObserverType& obs) {
			auto foundObs = std::find_if(std::begin(m_head), std::end(m_head), [&obs](const auto& o) {
				return &(*o) == &obs;
			});
			if (foundObs != std::end(m_head)) {
				m_head.erase(foundObs);
			} else {
				logger.template log<LogLevel::Error>() << "Trying to delete an observer but not found !";
			}
		}

		bool notifyObservers(T& t, Args... args) {
			auto hasBeenHandled = false;
			for (auto& obs : m_head) {
				hasBeenHandled |= obs->receive(t, std::forward<Args&>(args)...);
			}
			return hasBeenHandled;
		}


		virtual ~Observable() = default;

	private:
		std::vector<ObserverType*> m_head;
        static Logger<Observable<T, Args...>> logger;
	};
}

