#pragma once
#include "../WidgetPanelInteractive.h"
#include "../../Events/TimeEventListener.h"


namespace ska {
	class ScrollingMultiLineLabel : 
		public WidgetPanelInteractive<TimeEventListener> {
	public:
		void buildTextLines(const std::string& cs);
		// The text appears letter by letter (charactersPerSecond), line after line
		ScrollingMultiLineLabel(WidgetPanel& parent, const std::string& text, int fontSize, float charactersPerSecond, Point<int> relativePos);
		ScrollingMultiLineLabel& operator=(const ScrollingMultiLineLabel&) = delete;
		virtual ~ScrollingMultiLineLabel() = default;
	private:
		void refresh(unsigned int ellapsedTime);
		// The distance between two lines (pixels)
		int lineHeight() const;
		unsigned int m_currentLine;
		// Bytes of the current line shown, time not used yet to show the next letter
		std::size_t m_shown = 0;
		float m_time = 0.F;
		std::vector<std::string> m_textLines;
		const int m_fontSize;
		const float m_charactersPerSecond;
		bool m_stopped;
	};
}
