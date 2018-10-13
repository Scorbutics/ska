#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>

namespace ska {
    template <class T>
    class SharedFifo {
    private:
        std::deque<T> m_container;
        mutable std::mutex m_mutex;
        std::condition_variable m_dataCondition;

		SharedFifo& operator=(const SharedFifo&) = delete;
		SharedFifo(const SharedFifo&) = delete;
    public:
        SharedFifo() = default;
        SharedFifo& operator=(SharedFifo&&) = default;
        SharedFifo(SharedFifo&&) = default;

        void push(T&& item) {
            {
                std::lock_guard<std::mutex> lock { m_mutex };
                m_container.push_back(std::move(item));
            }
            m_dataCondition.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock { m_mutex };
            m_dataCondition.wait(lock, [this]() { return !m_container.empty(); });
            
            auto result = std::move(m_container.front());
            m_container.pop_front();
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
