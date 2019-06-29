#include "GUI/__internalConfig/LoggerConfig.h"
#include "BalloonDialog.h"
#include "../Components/Widget.h"
#include "../Components/Concrete/ScrollingMultiLineLabel.h"
#include "../GUI.h"
#include "Core/Utils/TimeUtils.h"

ska::BalloonDialog::BalloonDialog(ska::Widget& parent, const ska::Rectangle& box, const std::string& message, unsigned int delay, unsigned int fontSize) :
	WidgetPanelInteractive<ska::TimeEventListener>(parent, ska::Point<int>{ box.x, box.y }),
	m_delay(delay) {
	setWidth(box.w);
	setHeight(box.h);
	addWidget<ska::ScrollingMultiLineLabel>(message, fontSize, 2.F, ska::Point<int>(16, 16));
	m_startTime = ska::TimeUtils::getTicks();
}

bool ska::BalloonDialog::isExpired() const {
	return (ska::TimeUtils::getTicks() - m_startTime) >= m_delay;
}
