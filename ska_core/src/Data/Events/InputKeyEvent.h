#pragma once

namespace ska {
	class InputContextManager;

	/**
	* \brief Declares an event struct that is sent to observers when a keyboard event happens
	*/
	struct InputKeyEvent {
		explicit InputKeyEvent(const InputContextManager& icm_) :
			icm(icm_) {
		}

		void operator=(const InputKeyEvent&) = delete;
		virtual ~InputKeyEvent() = default;

		const InputContextManager& icm;
	};
}
