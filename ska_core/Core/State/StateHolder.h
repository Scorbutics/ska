#pragma once
#include "State.h"
#include "../../Task/TaskQueue.h"
#include "../../Data/Events/GameEventDispatcher.h"

namespace ska {

	/**
     * \brief Holds the current Game State.
     *
     * The main Game class contains the main loop program. In this loop, the StateHolder is called to be refreshed.
     * This class allows the current state to be stored in memory and make the whole workflow of the state (instanciation, loading, refreshing, unloading and destruction)
     * This is also the place where you can change your current game state.
     */
    class StateHolder :
		public TaskQueue {
    public:

	    /**
	     * \brief StateHolder constructor
	     * \param ged The event dispatcher used by the game.
	     *
	     * StateHolder needs a reference to the event dispatcher to emit an event when the current state changes
	     */
	    explicit StateHolder(GameEventDispatcher& ged);

		//No way you'll copy that !
		StateHolder(const StateHolder&) = delete;
		StateHolder& operator=(const StateHolder&) = delete;

    	virtual ~StateHolder() = default;


	    /**
		 * \brief Updates the graphic part of the current state
		 * \param ellapsedTime Time ellapsed during the last game loop
		 * \param drawables The current container of every drawable item (i.e. like a drawing queue)
		 */
		void graphicUpdate(unsigned int ellapsedTime, DrawableContainer& drawables);


	    /**
		 * \brief Updates the logic part of the current state
		 * \param ellapsedTime Time ellapsed during the last game loop
		 */
		void eventUpdate(unsigned int ellapsedTime);

	    /**
         * \brief Indicates the engine to prepare the loading of a next state.
         * \tparam S The real type of the state to be stored (automatically deduced by the param "state")
         * \param state The state to store (moved) in an unique_ptr
         */
        template<class S>
		void nextState(std::unique_ptr<S>&& state) {
			//static_assert(std::is_convertible<S, State>::value, "You must provide a state type / subtype as the next state of the state holder");
			m_nextState = std::move(state);
		}

	    /**
         * \brief Non virtual interface called by the engine.
         *
         * That is the function that manages every state changes, loading, unloading,
         */
        void update();

	private:
		StatePtr m_nextState;
		GameEventDispatcher& m_eventDispatcher;
		StatePtr m_currentState;
	    bool m_sceneLoaded;
		StatePtr m_lastState;
    };
}
