#pragma once
#include "Observer.h"

namespace ska {

	template <class T, class ...Args>
	class SubObserver :
		public Observer<T, Args...> {
	public:
		explicit SubObserver(std::function<bool(T&, Args...)> const& handler, Observable<T, Args...>& observable) :
			Observer<T, Args...>(handler),
			m_observable(observable) {
			m_observable.addObserver(*this);
		}

		virtual ~SubObserver() {
#ifdef _MSC_VER
			m_observable.ska::Observable<T, Args...>::removeObserver(static_cast<Observer<T, Args...>&>(*this));
#else
			m_observable.ska::Observable<T, Args...>::removeObserver(static_cast<Observer<T, Args...>&>(*this));
#endif
		}

	private:
		Observable<T, Args...>& m_observable;

	};
}
