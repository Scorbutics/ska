#pragma once

namespace ska {

    enum StateEventType {
        FIRST_STATE_LOADED,
		STATE_CHANGED
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
