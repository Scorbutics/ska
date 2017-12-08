#pragma once
#include <functional>

namespace ska {
	/**
	 * \brief Forces the implementation of the operator "()", meaning that sub-classes could be considered as functions.
	 */
	class Runnable {
	public:
		Runnable() = default;
		virtual ~Runnable() = default;

		/**
		* \brief If returns false, it means the Runnable has terminated. If returns true, the Runnable still has actions to perform.
		*/
		virtual bool operator()() = 0;
	};

	
	class RunnableLambda : public Runnable {
	public:
		RunnableLambda(std::function<bool()>&& callback) : 
			m_callback(callback) {
		}
		
		virtual ~RunnableLambda() = default;

		virtual bool operator()() override {
			return m_callback();
		}

	private:
		std::function<bool()> m_callback;
		
	};
}
