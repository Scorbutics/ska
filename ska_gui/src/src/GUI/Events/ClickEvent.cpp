#include "ClickEvent.h"
#include "ClickEventListener.h"
#include "GUI/Utils/WidgetMaskHelper.h"

ska::ClickEvent::ClickEvent(const MouseEventType& state, Point<int>& pos) : 
	HoverEvent(state, pos, pos, pos - pos) {
}


unsigned int ska::ClickEvent::getMask() const {
	return WidgetMaskEvent<ska::ClickEventListener>::MASK_ID();
}

SKA_DEFINE_GUI_EVENT(ska::ClickEventListener);
