#pragma once
#include <unordered_set>
#include <cassert>
#include "State.h"
#include "../../Task/Runnable.h"
#include "../../Task/TaskQueue.h"
#include "../../ECS/ISystem.h"
#include "../../Draw/IGraphicSystem.h"

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
		using StatePtr = std::unique_ptr<State>;
		using ISystemPtr = std::unique_ptr<ISystem>;
		using IGraphicSystemPtr = std::unique_ptr<IGraphicSystem>;

		using SystemContainer = std::vector<ISystemPtr>;
		using GraphicSystemContainer = std::vector<IGraphicSystemPtr>;

	public:
		StateBase();
		
		virtual void graphicUpdate(const unsigned int ellapsedTime, DrawableContainer& drawables) override final;
		virtual void eventUpdate(const unsigned int ellapsedTime) override final;

		void load(State* lastState) override final;
		bool unload() override final;

		void linkSubState(State& subState);
		void unlinkSubState(State& subState);

		void transmitLinkedSubstates(StateBase& state);

		State& addSubState(StatePtr s);
		bool removeSubState(State& subState);

		virtual ~StateBase() = default;

    protected:
		inline void addPriorizedLogic(const int priority, ISystemPtr system) {
			checkActiveState();
			addPriorizedLogic(m_logics, priority, std::move(system));
		}

		inline void addLogic(ISystemPtr system) {
			checkActiveState();
			addPriorizedLogic(m_logics, static_cast<int>(m_logics.size()), std::move(system));
		}
		
		inline void addPriorizedGraphic(const int priority, IGraphicSystemPtr graphicSystem) {
			checkActiveState();
			addPriorizedGraphic(m_graphics, priority, std::move(graphicSystem));
		}

		inline void addGraphic(IGraphicSystemPtr graphicSystem) {
			checkActiveState();
			addPriorizedGraphic(m_graphics, static_cast<int>(m_graphics.size()), std::move(graphicSystem));
		}

		Runnable& queueTask(RunnablePtr t) {
			return m_tasks.queueTask(std::move(t));
		}

		virtual void beforeLoad(State*) {}
		virtual void afterLoad(State*) {}

		virtual bool beforeUnload() { return false; }
		virtual bool afterUnload() { return false; }

		virtual void onGraphicUpdate(unsigned int, DrawableContainer& ) {}
		virtual void onEventUpdate(unsigned int ) {}


    private:

		void addPriorizedLogic(std::vector<ISystemPtr>& logics, const int priority, ISystemPtr system) const;
		void addPriorizedGraphic(std::vector<IGraphicSystemPtr>& graphics, const int priority, IGraphicSystemPtr system) const;

        inline bool waitTransitions() const {
			return !m_tasks.hasRunningTask();
		}


		inline void checkActiveState() const {
			assert(m_active && "State must be active before adding any system (put your code that adds systems in loads functions)");
        }

	    ska::TaskQueue m_tasks;

		SystemContainer m_logics;
		GraphicSystemContainer m_graphics;

		std::vector<StatePtr> m_subStates;
		std::unordered_set<std::reference_wrapper<State>, std::hash<State>> m_linkedSubStates;
        int m_state;
		bool m_active;

	};
}
