#pragma once
#include "../../Events/ValueChangedEvent.h"
#include "../../Events/ClickEventListener.h"
#include "../WidgetPanelInteractive.h"


namespace ska {
	class HorizontalSlider : public WidgetPanelInteractive<ValueChangedEventListener<float>, HoverEventListener, ClickEventListener> {
	public:
		HorizontalSlider(WidgetPanel& parent, const std::string& styleName, Point<int> relativePos, const unsigned int pixelWidth);
		
		float getValue() const;
		virtual void forceValue(float v);

		virtual ~HorizontalSlider() = default;

	private:
		float m_percents;
		float m_lastPercents;
		bool m_sliding;
	};
}
