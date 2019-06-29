#pragma once

#include "../WidgetPanelInteractive.h"
#include "../../Events/KeyEventListener.h"
#include "../../Events/FocusEventListener.h"
#include "../../Events/ValueChangedEventListener.h"
#include "../../Events/ClickEventListener.h"

namespace ska {
    class Label;

	class Input : public WidgetPanelInteractive<ValueChangedEventListener<std::wstring>, ClickEventListener, KeyEventListener, FocusEventListener> {
	public:
		Input(WidgetPanel& parent, const std::string& text, int fontSize, Point<int> relativePos);
		virtual ~Input() = default;

	private:
		void adaptDisplayWithText(Label& l);

		Rectangle m_clip;
		bool m_keyFocus;
		std::wstring m_rawText;
		std::wstring m_lastRawText;
	};
}
