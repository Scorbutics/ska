
#include "CheckBox.h"
#include "../../Events/ValueChangedEvent.h"
#include "Draw/Renderer.h"

ska::CheckBox::CheckBox(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const Rectangle* clip) :
	Button(parent, relativePos, placeHolderStyleName, clip, [&](Widget*, ClickEvent& e) {
		if (e.getState() == MOUSE_RELEASE) {
			m_value = !m_value;
			
			ValueChangedEvent<bool> vce(!m_value, m_value);
			directNotify(vce);
		}
	}),
	m_value(false),
	m_check(placeHolderStyleName + "_check.png") {
}

bool ska::CheckBox::getValue() const {
	return m_value;
}

void ska::CheckBox::forceValue(bool b) {
	m_value = b;
}

void ska::CheckBox::render(const Renderer& renderer) const {
	Button::render(renderer);
	if (m_value) {
		const auto& absPos = getAbsolutePosition();
		renderer.render(m_check, absPos.x + (getBox().w - m_check.getWidth()) / 2, absPos.y + (getBox().h - m_check.getHeight()) / 2);
	}
}

