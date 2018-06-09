#pragma once

#include "../../Point.h"

namespace ska {

	class BalloonDialog;

    enum class GUIEventType {
        REMOVE_WINDOW,
		ADD_BALLOON,
		REFRESH_BALLOON
    };


	/**
	* \brief Declares an event struct that is sent to observers when a GUI modification is asked programatically
	*/
	struct GUIEvent {
		explicit GUIEvent(const GUIEventType& t) : 
			balloonHandle(nullptr), 
			delay(0), 
			type(t) {
		}

		void operator=(const GUIEvent&) = delete;
		virtual ~GUIEvent() = default;

		BalloonDialog* balloonHandle;
		ska::Point<int> balloonPosition;
		int delay;
		std::string text;
		std::string windowName;
        const GUIEventType type;

	};
}
