#pragma once

#include <thread>
#include <memory>

#include "SharedFifo.h"

namespace ska {
	template <class Command>
    class ActiveObject;

	template <class Command>
    class ActiveObjectInner {
		template <class Command>
        friend class ActiveObject;
    private:
        
        ActiveObjectInner() = default;
        ActiveObjectInner(const ActiveObjectInner&) = delete;
        ActiveObjectInner& operator=(const ActiveObjectInner&) = delete;
        
        std::thread m_thread;
        bool m_done = false;
		SharedFifo<Command> m_messages;

        void run() {
            while(!m_done) {
                auto message = m_messages.pop();
                message();
            }
        }
    
    public:
        ActiveObjectInner(ActiveObjectInner&&) = default;
        ActiveObjectInner& operator=(ActiveObjectInner&&) = default;

        void send(Command callback) {
            m_messages.push(std::move(callback));
        }

        ~ActiveObjectInner() {
			send(Command{ [this]() {
				m_done = true;
			} });
            m_thread.join();
        }
        
    };

	template <class Command>
    class ActiveObject {
    public:
        ActiveObject() : inner(new ActiveObjectInner<Command>()) {
            inner->m_thread = std::thread(&ActiveObjectInner<Command>::run, inner.get());
        }

        void send(Command callback) {
            inner->send(std::move(callback));
        }
    private:
        std::unique_ptr<ActiveObjectInner<Command>> inner;
    };
}
