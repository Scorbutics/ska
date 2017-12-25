#include <limits>
#include "Button.h"
#include "../Widget.h"
#include "../../Events/ClickEventListener.h"
#include "Draw/Renderer.h"

ska::Button::Button(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const Rectangle* clip, ClickEventHandler const& callback) :
Hoverable<ValueChangedEventListener<bool>, ClickEventListener>(parent),
m_placeHolder(placeHolderStyleName + ".png"),
m_placeHolderHover(placeHolderStyleName + "_hover.png"),
m_placeHolderPressed(placeHolderStyleName + "_pressed.png"),
m_drawStyle(!placeHolderStyleName.empty()) {
	Widget::move(getRelativePosition() + relativePos);

	m_textureSelector = &m_placeHolder;
	m_lastTextureSelector = nullptr;
	
	const auto width = m_placeHolder.getWidth();
	const auto height = m_placeHolder.getHeight();
	if (width != 0 && height != 0) {
		Widget::setWidth(width);
		Widget::setHeight(height);
	} else {
		Widget::setWidth(10);
		Widget::setHeight(10);
	}

	initHandlers();
	addHandler<ClickEventListener>(callback);

	if (clip != nullptr) {
		m_clip = *clip;
	} else {
		m_clip = Rectangle{0, 0, 0, 0};
		m_clip.w = std::numeric_limits<int>().max();
		m_clip.h = std::numeric_limits<int>().max();
	}

}

ska::Button::Button() :
m_drawStyle(false) {
	m_textureSelector = &m_placeHolder;
	m_lastTextureSelector = nullptr;
	m_clip = Rectangle{0, 0, 0, 0};
	m_clip.w = std::numeric_limits<int>().max();
	m_clip.h = std::numeric_limits<int>().max();

	initHandlers();
}

ska::Button::Button(Widget& parent) :
Hoverable<ValueChangedEventListener<bool>, ClickEventListener>(parent),
m_drawStyle(false) {
	m_textureSelector = &m_placeHolder;
	m_lastTextureSelector = nullptr;
	m_clip = Rectangle{0, 0, 0, 0};
	m_clip.w = std::numeric_limits<int>().max();
	m_clip.h = std::numeric_limits<int>().max();

	initHandlers();
}

void ska::Button::switchTextureAndMemorize() {
	Texture* t;
	switch (m_state) {
		case ButtonState::HOVER:
		case ButtonState::ENTER:
			t = &m_placeHolderHover;
			break;

		case ButtonState::NONE:
		case ButtonState::DISABLED:
			t = &m_placeHolder;
			break;

		case ButtonState::PRESSED:
			t = &m_placeHolderPressed;
			break;

		default:
			t = &m_placeHolder;
			break;

	}
	m_lastTextureSelector = m_textureSelector;
	m_textureSelector = t;
}

void ska::Button::resetTexture() {
	m_textureSelector = m_lastTextureSelector;
}


void ska::Button::initHandlers() {
	addHeadHandler<ClickEventListener>([this](Widget*, ClickEvent& e) {
		auto handled = false;
		switch (e.getState()) {
		case MOUSE_CLICK:
			m_state = ButtonState::PRESSED;
			switchTextureAndMemorize();
			handled = true;
			break;
		case MOUSE_RELEASE:
			m_state = ButtonState::HOVER;
			resetTexture();
			handled = true;
			break;
		default:
			break;
		}

		if (handled) {
			e.setTarget(this);
		} else {
			e.stopPropagation(STOP_CALLBACK);
		}
	});
}

void ska::Button::forceState(ButtonState::Enum e) {
	m_state = e;
	switchTextureAndMemorize();
}

void ska::Button::render(const Renderer& renderer) const {

	if (!m_drawStyle || m_textureSelector == nullptr) {
		return;
	}

	const auto& pos = getAbsolutePosition();
	if (m_clip.w == std::numeric_limits<int>().max()) {
		renderer.render(*m_textureSelector, pos.x, pos.y);
	} else {
		renderer.render(*m_textureSelector, pos.x, pos.y, &m_clip);
	}
}



ska::Button::~Button() { }

