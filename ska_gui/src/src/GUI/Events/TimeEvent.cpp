#include "TimeEvent.h"
#include "TimeEventListener.h"
#include "../Utils/WidgetMaskHelper.h"

ska::TimeEvent::TimeEvent(unsigned ellapsedTime) : 
	WidgetEvent<TimeEvent, TimeEventListener>(),
	m_ellapsedTime(ellapsedTime) {
}


unsigned ska::TimeEvent::getMask() const {
	return WidgetMaskEvent<TimeEventListener>::MASK_ID();
}

bool ska::TimeEvent::affects(const Widget& ) const {
	return true;
}

ska::StopType ska::TimeEvent::stopped() const {
	return NOT_STOPPED;
}

unsigned ska::TimeEvent::getEllapsedTime() const {
	return m_ellapsedTime;
}

SKA_DEFINE_GUI_EVENT(ska::TimeEventListener);
