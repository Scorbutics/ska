#pragma once
#include <memory>
#include <cstdint>
namespace ska {
	class DrawableContainer;

	class State;

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

		State(State&&) = default;

		State(const State&) = delete;
		bool operator=(const State&) = delete;

		virtual void graphicUpdate(unsigned ellapsedTime, DrawableContainer& drawables) = 0;
		virtual void eventUpdate(unsigned int) = 0;
		virtual void loadBefore(State* lastState) = 0;
		virtual void loadAfter(State* lastState) = 0;
		virtual bool unload() = 0;

		virtual ~State() = default;

	};


	inline bool operator==(const State& val1, const State& val2) {
		return &val1 == &val2;
	}

}

namespace std {
	template <>
	struct hash<ska::State> {
		size_t operator()(const ska::State& s) const {
			return std::hash<std::uintptr_t>()(reinterpret_cast<std::uintptr_t>(&s));
		}
	};

}