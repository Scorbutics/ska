#pragma once

#include "../../Point.h"

namespace ska {
	
	enum class DebugGUIEventType {
		ADD_DEBUG_INFO,
		REMOVE_DEBUG_INFO
	};

	struct DebugGUIEvent {
		using Refresher = std::function<std::string()>;

		explicit DebugGUIEvent(const DebugGUIEventType& t, Refresher r) :
			type(t),
			refresher(std::move(r)) {
		}

		void operator=(const DebugGUIEvent&) = delete;
		virtual ~DebugGUIEvent() = default;

		unsigned int delay = 500u;
		std::string text;
		Refresher refresher;
		const DebugGUIEventType type;

	};
}
