#pragma once

namespace ska {
	class InputContextManager;

	/**
	* \brief Declares an event struct that is sent to observers when there is a mouse action
	*/
	struct InputMouseEvent {
		explicit InputMouseEvent(InputContextManager& icm_) :
			icm(icm_) {
		}

		void operator=(const InputMouseEvent&) = delete;
		virtual ~InputMouseEvent() = default;

		InputContextManager& icm;
	};
}
