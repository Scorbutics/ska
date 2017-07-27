#include "BalloonDialog.h"
#include "../Components/Widget.h"
#include "../Components/Concrete/ScrollingMultiLineLabel.h"
#include "../GUI.h"
#include "Utils/TimeUtils.h"

ska::BalloonDialog::BalloonDialog(ska::Widget& parent, const ska::Rectangle& box, const std::string& message, unsigned int delay, unsigned int fontSize) :
	WindowIG<ska::TimeEventListener>(parent, box, ska::GUI::MENU_DEFAULT_THEME_PATH + "balloon"),
	m_delay(delay) {
	addWidget<ska::ScrollingMultiLineLabel>(message, fontSize, 2.F, ska::Point<int>(16, 16));
	m_startTime = ska::TimeUtils::getTicks();
}

bool ska::BalloonDialog::isExpired() const {
	return (ska::TimeUtils::getTicks() - m_startTime) >= m_delay;
}
