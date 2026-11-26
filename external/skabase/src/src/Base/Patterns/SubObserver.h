#pragma once
#include "Observer.h"

namespace ska {
	template <class T, template<typename T1, typename...> class Container, template <typename T1> class ObserverT>
	class Observable;

	template <class T, template<typename T1, typename...> class Container = std::vector, template <typename T2> class ObserverT = Observer>
	class SubObserver :
		public ObserverT<T> {
			using ObservableType = Observable<T, Container, ObserverT>;
	public:
		template <class ... Args>
		explicit SubObserver(std::function<bool(T&)> const& handler, ObservableType& observable, Args&& ... args) :
			ObserverT<T>(std::forward<Args>(args)..., handler),
			m_observable(observable) {
			m_observable.ObservableType::addObserver(static_cast<ObserverT<T>&>(*this));
		}

		virtual ~SubObserver() {
			m_observable.ObservableType::removeObserver(static_cast<ObserverT<T>&>(*this));
		}

	private:
		ObservableType& m_observable;
	};
}
