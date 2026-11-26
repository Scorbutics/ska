#pragma once
#include <Base/Patterns/Observable.h>
#include <Base/Patterns/SubObserver.h>
#include "unique_buffered_priority_indexed_fixed_queue.h"

namespace ska {

	struct PriorityObserverGetter {
		template<class T>
		std::size_t operator()(const T& value) const {
			return value->priority;
		}
	};

	template <class T>
	class PriorityObserver : public Observer<T> {
		friend struct PriorityObserverGetter;
    	
	protected:
    	template <class ...Args>
    	PriorityObserver(int p, Args&& ... args) : Observer<T>::Observer(std::forward<Args>(args)...), priority(p) {}
    	
	private:
    	int priority;
	};

	template <class T>
	using priority_queue_for_observer = unique_buffered_priority_indexed_fixed_queue<T, 10, PriorityObserverGetter>;

	template <class T>
	using observable_priority_queue = Observable<T, priority_queue_for_observer, PriorityObserver>;

	template <class T>
	using subobserver_priority_queue = SubObserver<T, priority_queue_for_observer, PriorityObserver>;
}