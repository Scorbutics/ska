#include "Widget.h"
#include "WidgetPanel.h"

ska::Widget::Widget(WidgetPanel& parent) :
	m_parent(&parent) {
}

ska::Widget::Widget(WidgetPanel& parent, const Point<int>& position) :
	m_parent(&parent) {
	m_box.x = position.x;
	m_box.y = position.y;
}

bool ska::Widget::isVisible() const {
	return m_visible;
}

void ska::Widget::setWidth(unsigned int w) {
	m_box.w = w;
}

void ska::Widget::setHeight(unsigned int h) {
	m_box.h = h;
}

void ska::Widget::show(bool sh) {	
	const auto willResort = m_parent != nullptr && (m_visible ^ sh);
	m_visible = sh;
	if (willResort) {
		m_parent->resort();
	}
}

void ska::Widget::move(const Point<int>& pos) {
	m_box.x = pos.x;
	m_box.y = pos.y;
}

ska::WidgetPanel* ska::Widget::getParent() const {
	return m_parent;
}

bool ska::Widget::isAParent(const Widget& w) const {
	if (static_cast<const void*>(this) == static_cast<const void*>(&w)) {
		return true;
	}

	return w.getParent() == nullptr ? false : isAParent(*w.getParent());
}


const ska::Rectangle& ska::Widget::getBox() const {
	return m_box;
}

ska::Point<int> ska::Widget::getAbsolutePosition() const {
	if (m_parent == nullptr) {
		return m_box;
	}

	return m_parent->getAbsolutePosition() + m_box;
}

ska::Point<int> ska::Widget::getRelativePosition() const {
	return m_box;
}

void ska::Widget::focus(bool f) {
	m_focused = f;
}

bool ska::Widget::isFocused() const {
	return m_focused;
}

void ska::Widget::setPriority(int priority) {
	DrawableFixedPriority::setPriority(priority);
	if (m_parent != nullptr) {
		m_parent->resort();
	}
}
