#pragma once
#include <functional>
#include <cassert>
#include "../Exceptions/IllegalArgumentException.h"

namespace ska {
    template <typename T, template<typename T1, typename...> class Container = std::vector>
	class Observable;

	template <class T>
	class Observer {
    public:
        template <typename T1, template<typename T2, typename...> class Container>
		friend class Observable;

		explicit Observer(std::function<bool(T&)> handler) : 
			m_handler(std::move(handler)) {
			assert(m_handler != nullptr && "Function cannot be null");
		}

		virtual ~Observer() = default;

	private:
		std::function<bool(T&)> m_handler;

		bool receive(T& evt) {
			return (m_handler)(evt);
		}

	};
}
