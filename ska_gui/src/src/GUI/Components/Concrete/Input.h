#pragma once

#include "../WidgetPanel.h"
#include "../../Events/KeyEventListener.h"
#include "../../Events/FocusEventListener.h"
#include "../../Events/ValueChangedEventListener.h"
#include "../../Events/ClickEventListener.h"

namespace ska {
    class Label;

	class Input : public WidgetPanel<ValueChangedEventListener<std::wstring>, ClickEventListener, KeyEventListener, FocusEventListener> {
	public:
		Input(Widget& parent, const std::string& text, int fontSize, Point<int> relativePos);
		virtual ~Input() = default;

	private:
		void adaptDisplayWithText(Label& l);

		Rectangle m_clip;
		bool m_keyFocus;
		std::wstring m_rawText;
		std::wstring m_lastRawText;
	};
}
