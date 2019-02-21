#pragma once

#include <vector>
#include <algorithm>
#include "Observer.h"
#include "../LoggerConfig.h"

namespace ska {
	namespace detail::observable {
		template<typename C, typename T>
		inline auto insert_in_container(C& c, T&& t) ->
			decltype(c.insert(std::forward<T>(t)), void()) {
			c.insert(std::forward<T>(t));
		}

		template<typename C, typename T>
		inline auto insert_in_container(C& c, T&& t) ->
			decltype(c.push(std::forward<T>(t)), void()) {
			c.push(std::forward<T>(t));
		}

		template<typename C, typename T>
		inline auto insert_in_container(C& c, T&& t) ->
			decltype(c.insert(std::forward<T>(t)), void()) {
			c.insert(std::forward<T>(t));
		}
	}

	template <class T, template<typename T, typename...> class Container = std::vector>
	class Observable {
		using ObserverType = Observer<T>;
	public:
		Observable() = default;
		virtual ~Observable() = default;

		void addObserver(ObserverType& obs) {
			detail::observable::insert_in_container(m_head, obs);
		}

		void removeObserver(ObserverType& obs) {
			static Logger<> logger;
            auto foundObs = std::find_if(std::begin(m_head), std::end(m_head), [&obs](const auto& o) {
				return &(*o) == &obs;
			});
			if (foundObs != std::end(m_head)) {
				m_head.erase(foundObs);
			} else {
				logger.log<LogLevel::Error, decltype(*this)>() << "Trying to delete an observer but not found !";
			}
		}

		bool notifyObservers(T& t) {
			auto hasBeenHandled = false;
			for (auto& obs : m_head) {
				hasBeenHandled |= obs->receive(t);
			}
			return hasBeenHandled;
		}

	private:
		Container<ObserverType*> m_head;
        
	};
}

