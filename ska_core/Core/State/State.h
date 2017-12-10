#pragma once
#include <memory>
#include <functional>

namespace ska {
	class StateHolder;
	class ISystem;
	class IGraphicSystem;
	class DrawableContainer;

	class State;
	using StatePtr = std::unique_ptr<State>;

	/**
	 * \brief Represents a Game State.
	 * 
	 * A Game state is the main holder of game mechanics.
	 * The state object contains each system of the Entity Component System (You can add them)
	 * A state can be loaded and then unloaded. 
	 * The workflow is the next one : 
	 *  - State constructor is called
	 *  - The engine calls the "load" function (that's mainly here where you're going to add logic and graphic systems, manage transitions)
	 *  - The current state is updated in the main loop, graphics and logics
	 *  - When the current state changes, the "unload" function is called (transition out)
	 *  - State destructor is called
	 *  
	 *  StateBase implements a way that "load" and "unload" are generic and there are "beforeLoad", "afterLoad", "beforeUnload", "afterUnload" functions.
	 *  See "StateBase"
	 */
	class State {
	public:
		State() = default;

		virtual void graphicUpdate(unsigned ellapsedTime, DrawableContainer& drawables) = 0;
		virtual void eventUpdate(unsigned int) = 0;
		virtual void load(StatePtr* lastState) = 0;
		virtual bool unload() = 0;
		virtual StateHolder& getHolder() = 0;

		virtual ~State() = default;

	};

	
}

