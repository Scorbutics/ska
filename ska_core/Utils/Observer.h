#pragma once
#include <functional>
#include <cassert>
#include "../Exceptions/IllegalArgumentException.h"

namespace ska {
    template <typename T, typename ...Args>
	class Observable;


	template <class T, class ...Args>
	class Observer {
    public:
        template <typename T1, typename ...Args1>
		friend class Observable;

		explicit Observer(std::function<bool(T&, Args...)> handler) : 
			m_handler(std::move(handler)) {
			assert(m_handler != nullptr && "Function cannot be null");
		}

		virtual ~Observer() = default;

	private:
		std::function<bool(T&, Args...)> m_handler;

		bool receive(T& evt, Args&... args) {
			return (m_handler)(evt, std::forward<Args&>(args)...);
		}

	};
}
