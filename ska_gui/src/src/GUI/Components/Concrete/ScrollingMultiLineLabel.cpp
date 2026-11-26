#include "ScrollingMultiLineLabel.h"
#include "Label.h"
#include "Base/Values/Strings/StringUtils.h"

namespace {
	// The length of the text up to the next character (a UTF-8 character is one or several bytes)
	std::size_t NextCharacter(const std::string& text, std::size_t length) {
		length++;
		while (length < text.size() && (static_cast<unsigned char>(text[length]) & 0xC0) == 0x80) {
			length++;
		}
		return length;
	}
}

ska::ScrollingMultiLineLabel::ScrollingMultiLineLabel(WidgetPanel& parent, const std::string& text, int fontSize, float charactersPerSecond, Point<int> relativePos) :
	WidgetPanelInteractive(parent, relativePos), m_currentLine(0),
	m_fontSize(fontSize),
	m_charactersPerSecond(charactersPerSecond < 1.F ? 1.F : charactersPerSecond), m_stopped(false) {

	buildTextLines(text);

	Widget::setHeight(static_cast<unsigned int>(static_cast<int>(m_textLines.size()) * lineHeight()));

	// The first line, empty : its letters appear one by one
	addWidget<Label>(" ", m_fontSize, Point<int>());
	addHandler<TimeEventListener>([&](Widget*, TimeEvent& te) {
		refresh(te.getEllapsedTime());
	});
}

void ska::ScrollingMultiLineLabel::refresh(unsigned int ellapsedTime) {
	if(m_stopped) {
		return;
	}
	if (m_textLines.empty()) {
		m_stopped = true;
		return;
	}
	m_time += static_cast<float>(ellapsedTime);
	const auto delay = 1000.F / m_charactersPerSecond;
	auto changed = false;
	while (m_time >= delay && !m_stopped) {
		m_time -= delay;
		const auto& line = m_textLines[m_currentLine];
		if (m_shown < line.size()) {
			m_shown = NextCharacter(line, m_shown);
			changed = true;
			continue;
		}
		// The line is complete : the next one
		if (m_currentLine + 1 >= m_textLines.size()) {
			m_stopped = true;
			break;
		}
		m_currentLine++;
		m_shown = 0;
		addWidget<Label>(" ", m_fontSize, Point<int>(0, lineHeight() * static_cast<int>(m_currentLine)));
	}
	if (changed || m_shown == 0) {
		const auto& line = m_textLines[m_currentLine];
		auto& label = static_cast<Label&>(*getWidget(m_currentLine));
		label.modifyText(m_shown == 0 ? std::string{ " " } : line.substr(0, m_shown));
	}
}

int ska::ScrollingMultiLineLabel::lineHeight() const {
	// A quarter of the size between two lines : the letters going down of a line (g, p, q) do not touch the next one
	return m_fontSize + m_fontSize / 4;
}

void ska::ScrollingMultiLineLabel::buildTextLines(const std::string& cs) {
	m_textLines = StringUtils::split(cs, '\n');
}
