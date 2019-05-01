#pragma once
#include "../Components/WidgetPanel.h"
#include "../Events/TimeEventListener.h"

namespace ska {
	class Widget;

	class BalloonDialog : public WidgetPanel<TimeEventListener> {

	public:
		BalloonDialog(Widget& parent, const ska::Rectangle& box, const std::string& message, unsigned int delay, unsigned int fontSize);
		~BalloonDialog() = default;
		bool isExpired() const;

	private:
		unsigned int m_startTime;
		const unsigned int m_delay;
	};
}
