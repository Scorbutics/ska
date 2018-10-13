#pragma once

#include <thread>
#include <functional>
#include <memory>

#include "SharedStack.h"

namespace ska {
    class ActiveObject;

    class ActiveObjectInner {
        friend class ActiveObject;
    private:
        using Command = std::function<void()>;
        
        ActiveObjectInner() = default;
        ActiveObjectInner(const ActiveObjectInner&) = delete;
        ActiveObjectInner& operator=(const ActiveObjectInner&) = delete;
        
        std::thread m_thread;
        bool m_done = false;
        SharedStack<Command> m_messages;

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
            send([this]() {
                m_done = true;
            });
            m_thread.join();
        }
        
    };

    class ActiveObject {
    public:
        ActiveObject() : inner(new ActiveObjectInner()) {
            inner->m_thread = std::thread(&ActiveObjectInner::run, inner.get());
        }

        void send(ActiveObjectInner::Command callback) {
            inner->send(std::move(callback));
        }
    private:
        std::unique_ptr<ActiveObjectInner> inner;
    };
}
