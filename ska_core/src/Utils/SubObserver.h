#pragma once
#include "Observer.h"

namespace ska {

	template <class T, template<typename T1, typename...> class Container = std::vector>
	class SubObserver :
		public Observer<T> {
	public:
		explicit SubObserver(std::function<bool(T&)> const& handler, Observable<T, Container>& observable) :
			Observer<T>(handler),
			m_observable(observable) {
			m_observable.addObserver(*this);
		}

		virtual ~SubObserver() {
			m_observable.Observable<T, Container>::removeObserver(static_cast<Observer<T>&>(*this));
		}

	private:
		Observable<T>& m_observable;
	};
}
