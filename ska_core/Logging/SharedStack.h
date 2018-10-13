#pragma once

#include <stack>
#include <mutex>
#include <condition_variable>

namespace ska {
    template <class T>
    class SharedStack {
    private:
        std::stack<T> m_container;
        mutable std::mutex m_mutex;
        std::condition_variable m_dataCondition;

        SharedStack& operator=(const SharedStack&) = delete;
        SharedStack(const SharedStack&) = delete;
    public:
        SharedStack() = default;
        SharedStack& operator=(SharedStack&&) = default;
        SharedStack(SharedStack&&) = default;

        void push(T item) {
            {
                std::lock_guard<std::mutex> lock { m_mutex };
                m_container.push(std::move(item));
            }
            m_dataCondition.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock { m_mutex };
            m_dataCondition.wait(lock, [this]() { return !m_container.empty(); });
            
            auto result = std::move(m_container.top());
            m_container.pop();
            return result;
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock { m_mutex };
            return m_container.empty();
        }

        std::size_t size() const {
            std::lock_guard<std::mutex> lock { m_mutex };
            return m_container.size();
        }
    };
}
