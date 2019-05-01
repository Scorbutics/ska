#pragma once
#include <functional>
#include "Runnable.h"
#include "../Utils/Identity.h"

namespace ska {
	
	/**
	 * \brief Materializes a thing to do, in loop, until some requirement is met (by returning "true" or "false" when ran for example)
	 * 
	 * The main difference with its parent class Runnable is that it holds a lambda function that is executed as operator "()".
	 * In other terms, it provides a way of implementing Runnable.
	 */
	class Task : 
		public Runnable {
	public:

        using F = std::function<bool(Task&)>;
        using LAMBDA = meta::Identity<F>::type;

		explicit Task(LAMBDA const& f) :
			m_f(f) {
		}

		virtual bool operator()() final override {
			return run();
		}

		virtual ~Task() {
		}

	private:
		bool run() {
			return m_f(*this);
		}

		F m_f;
	};

}
