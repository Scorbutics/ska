#pragma once
#include <unordered_set>
#include "State.h"
#include "StateHolder.h"
#include "../../Draw/IGraphicSystem.h"
#include "../../ECS/ISystem.h"
#include "StateBuilder.h"
#include <cassert>

namespace ska {

	/**
	 * \brief Base State class.
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
	class StateBase : public State {

	public:
		StateBase();
		
		virtual void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables) override final;
		virtual void eventUpdate(unsigned int ellapsedTime) override final;

		void load(StatePtr* lastState) override final;
		bool unload() override final;

		void linkSubState(State& subState);
		void unlinkSubState(State& subState);

		template<class State>
		void transmitLinkedSubstates(State& state) {
			m_linkedSubStates = state.m_linkedSubStates;
		}

		template<class State, class ...Args>
		State* addSubState(Args&& ... args) {
			auto s = std::make_unique<State>(std::forward<Args>(args)...);
            auto result = static_cast<State*>(s.get());
            m_subStates.push_back(std::move(s));
            auto state = result;
			
			//manages the case that this current state isn't loaded yet : then substate should be loaded after the state is loaded
			if (m_active) {
				state->load(nullptr);
			}

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
			checkActiveState();
			return m_builder.addPriorizedLogic<System, Args...>(m_logics, priority, std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		System* addLogic(Args&& ... args) {
			checkActiveState();
			return this->m_builder.addPriorizedLogic<System, Args...>(m_logics, static_cast<int>(m_logics.size()), std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		System* addPriorizedGraphic(int priority, Args&& ... args) {
			checkActiveState();
			return m_builder.addPriorizedGraphic<System, Args...>(m_graphics, priority, std::forward<Args>(args)...);
		}

		template<class System, class ...Args>
		System* addGraphic(Args&& ... args) {
			checkActiveState();
			return this->m_builder.addPriorizedGraphic<System, Args...>(m_graphics, static_cast<int>(m_graphics.size()), std::forward<Args>(args)...);
		}

		template <class T>
		ska::Runnable& queueTask(std::unique_ptr<T>&& t) {
			return m_tasks.queueTask(std::forward<std::unique_ptr<T>>(t));
		}

		virtual void beforeLoad(StatePtr*) {}
		virtual void afterLoad(StatePtr*) {}

		virtual bool beforeUnload() { return false; }
		virtual bool afterUnload() { return false; }

		virtual void onGraphicUpdate(unsigned int, DrawableContainer& ) {}
		virtual void onEventUpdate(unsigned int ) {}


    private:
        inline bool waitTransitions() const {
			return !m_tasks.hasRunningTask();
		}


		inline void checkActiveState() const {
			assert(m_active && "State must be active before adding any system (put your code that adds systems in loads functions)");
        }

	    ska::TaskQueue m_tasks;
		StateBuilder m_builder;

		std::vector<std::unique_ptr<ISystem>> m_logics;
		std::vector<std::unique_ptr<IGraphicSystem>> m_graphics;

		std::vector<std::unique_ptr<State>> m_subStates;
		std::unordered_set<State*> m_linkedSubStates;
        int m_state;
		bool m_active;

	};
}
