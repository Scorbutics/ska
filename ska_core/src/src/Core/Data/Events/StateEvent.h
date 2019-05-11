#pragma once

namespace ska {

    enum class StateEventType {
        FIRST_STATE_LOAD,
		STATE_CHANGE
    };

	/**
	* \brief Declares an event struct that is sent to observers when the current state has a modification
	*/
	struct StateEvent {
		explicit StateEvent(const StateEventType& t) : 
			type(t) {
		}

		void operator=(const StateEvent&) = delete;
		virtual ~StateEvent() = default;

        const StateEventType type;

	};
}
