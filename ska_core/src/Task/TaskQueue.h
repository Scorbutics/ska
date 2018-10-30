#pragma once
#include <queue>
#include <memory>
#include "Runnable.h"
#include <functional>

namespace ska {
	using RunnablePtr = std::unique_ptr<Runnable>;

	/**
	 * \brief Holds and executes tasks.
	 * 
	 * It ensures that a task is terminated before running the next one.
	 * Meaning that even if it takes several loops to execute a task, only the current one will be executed each loop.
	 */
	class TaskQueue {
	public:
		TaskQueue();

		//No way you'll copy that
		TaskQueue(const TaskQueue&) = delete;
		TaskQueue& operator=(const TaskQueue&) = delete;

		virtual ~TaskQueue() = default;

		/**
		 * \brief Informs the caller about remaining current running task(s) or not.
		 * \return true if there is at least one task running, false otherwise.
		 */
		bool hasRunningTask() const;

		/**
		 * \brief Adds a task after the last added one in the running pipe.
		 * \param t Task instance to queue (moved)
		 * \return The added task materialized by a Runnable instance
		 */
		Runnable& queueTask(RunnablePtr t) {
            m_tasks.push(move(t));
            return *m_tasks.back();
		}

		Runnable& queueTask(std::function<bool()>&& t) {
			m_tasks.push(std::make_unique<RunnableLambda>(std::forward<std::function<bool()>>(t)));
			return *m_tasks.back();
		}

		/**
		 * \brief Called by the main loop engine, runs the current task and changes it if it's terminated.
		 */
		void refresh();

	private:
		std::queue<RunnablePtr> m_tasks;
		RunnablePtr m_previous;
		RunnablePtr m_current;
	};
}
