#pragma once
#include <unordered_set>
#include "State.h"
#include "StateHolder.h"
#include "../../Draw/IGraphicSystem.h"
#include "../../ECS/ISystem.h"
#include "StateBuilder.h"

namespace ska {
	class Window;
	class InputContextManager;

	/**
	 * \brief Templated version of the base State class.
	 * \tparam EM The EntityManager class
	 *
	 * This class implements an easy way to manage transitions between states.
	 * During class loading, it allows the user to queue a task. When he does it, the task end is waited before the State truly load.
	 * More than State, StateBase can also have sub-states and manages it.
	 * Rules about sub-states are those :
	 *  - Its can be added / removed at any time
	 *  - When added, a substate is loaded
	 *  - When removed, a substate is unloaded
	 *  - Main state is refreshed, then sub-states are
	 *  - Loading and unloading of its are done between befores and afters state parent load / unload function
	 *  - You can transfer substates to another state when the state changes
	 */
	template <class EM>
	class StateBase : public State {

	public:
		StateBase(EM& em) :
			m_builder(em),
			m_entityManager(em),
			m_state(0) {
		}
		virtual void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) override final {
			onGraphicUpdate(ellapsedTime, drawables);

			/* Graphics */
			for (auto& s : m_subStates) {
				s->graphicUpdate(ellapsedTime, drawables);
			}

			for (auto& s : m_linkedSubStates) {
				s->graphicUpdate(ellapsedTime, drawables);
			}

			for (auto& s : m_graphics) {
				s->setDrawables(drawables);
				s->update(ellapsedTime);
			}
		}

		virtual void eventUpdate(unsigned int ellapsedTime) override final {
			onEventUpdate(ellapsedTime);

			/* Logics */
			for (auto& s : m_subStates) {
				s->eventUpdate(ellapsedTime);
			}

			for (auto& s : m_linkedSubStates) {
				s->eventUpdate(ellapsedTime);
			}

			for (auto& s : m_logics) {
				s->update(ellapsedTime);
			}
		}

		void load(StatePtr* lastState) override final {
			beforeLoad(lastState);
			m_state = 1;

			for (auto& s : m_subStates) {
				s->load(lastState);
			}

			for (auto& s : m_linkedSubStates) {
				s->load(lastState);
			}

			m_state = 2;
			afterLoad(lastState);
			m_state = 3;
		}

		bool unload() override final {
		    m_tasks.refresh();
			if (m_state == 3) {
				auto beforeUnloaded = !beforeUnload();
				if(beforeUnloaded) {
					m_state = 2;
				}
			}

			//If main scene beforeUnload is finished, THEN we can unload subscenes
			if (m_state == 2) {
				auto wTransitions = waitTransitions();
				if (wTransitions) {
					m_state = 1;
				}
			}

			if(m_state == 1) {
				auto subscenesUnloaded = true;
				for (auto& s : m_subStates) {
					subscenesUnloaded &= !s->unload();
				}

				for (auto& s : m_linkedSubStates) {
					subscenesUnloaded &= !s->unload();
				}

				if (subscenesUnloaded) {
					m_state = 0;
				}
			}

			//If everything is unloaded, THEN we can call main scene afterUnload
			if(m_state == 0) {
				auto afterUnloaded = !afterUnload();
				if (afterUnloaded) {
					m_state = -1;
				}
			}

			if(m_state == -1) {
				auto wTransitions = waitTransitions();
				if (wTransitions) {
					m_state = -2;
				}
			}

			return m_state != -2;
		}

		void linkSubState(State& subState) {
			m_linkedSubStates.insert(&subState);
		}

		void unlinkSubState(State& subState) {
			m_linkedSubStates.erase(&subState);
		}

		template<class State>
		void transmitLinkedSubstates(State& state) {
			m_linkedSubStates = state.m_linkedSubStates;
		}

		template<class State, class ...Args>
		State* addSubState(Args&& ... args) {
			auto s = std::make_unique<State>(m_entityManager, std::forward<Args>(args)...);
            auto result = static_cast<State*>(s.get());
            m_subStates.push_back(std::move(s));
            auto state = result;
			state->load(nullptr);
			return state;
		}

		template<class StateT>
		bool removeSubState(StateT& subState) {
            auto it = std::remove_if(m_subStates.begin(), m_subStates.end(), [&subState](const auto& c) {
                return c.get() == &subState;
            });

            auto removedState = std::move(*it);
            m_subStates.erase(it, m_subStates.end());
            auto removed = std::move(removedState);
			removed->unload();
			return true;
		}

		virtual ~StateBase() = default;

    protected:
		template<class System, class ...Args>
		System* addPriorizedLogic(int priority, Args&& ... args) {
			return m_builder.addPriorizedLogic<System, Args...>(m_logics, priority, std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		System* addLogic(Args&& ... args) {
			return this->m_builder.addPriorizedLogic<System, Args...>(m_logics, static_cast<int>(m_logics.size()), std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		System* addPriorizedGraphic(int priority, Args&& ... args) {
			return m_builder.addPriorizedGraphic<System, Args...>(m_graphics, priority, std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		System* addGraphic(Args&& ... args) {
			return this->m_builder.addPriorizedGraphic<System, Args...>(m_graphics, static_cast<int>(m_graphics.size()), std::forward<Args>(args)...);
		}

		template <class T>
		ska::Runnable& queueTask(std::unique_ptr<T>&& t) {
			return m_tasks.queueTask(std::forward<std::unique_ptr<T>>(t));
		}

		virtual void beforeLoad(StatePtr*) {
		}

		virtual void afterLoad(StatePtr*) {
		}

		virtual bool beforeUnload() {
			return false;
		}

		virtual bool afterUnload() {
			return false;
		}

		virtual void onGraphicUpdate(unsigned int, DrawableContainer& ) {
		}

		virtual void onEventUpdate(unsigned int ) {
		}


    private:
        bool waitTransitions() const {
			return !m_tasks.hasRunningTask();
		}

	    ska::TaskQueue m_tasks;
		StateBuilder<EM> m_builder;
        EM& m_entityManager;

		std::vector<std::unique_ptr<ISystem>> m_logics;
		std::vector<std::unique_ptr<IGraphicSystem>> m_graphics;

		std::vector<std::unique_ptr<State>> m_subStates;
		std::unordered_set<State*> m_linkedSubStates;
        int m_state;

	};
}
