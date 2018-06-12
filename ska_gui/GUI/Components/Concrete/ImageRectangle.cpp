#include <limits>
#include "ImageRectangle.h"
#include "Draw/Renderer.h"

ska::ImageRectangle::ImageRectangle(Widget& parent, Rectangle box, Color c) :
	Widget(parent, ska::Point<int>{box.x, box.y}) {
	m_img.loadFromColoredRect(box.w, box.h, std::move(c));
	m_box = box;

	ImageRectangle::setWidth(box.w);
	ImageRectangle::setHeight(box.h);
}

void ska::ImageRectangle::setWidth(unsigned int w) {
	Widget::setWidth(w);
	m_img.resize(w, m_img.getHeight());
}

void ska::ImageRectangle::setHeight(unsigned int h) {
	Widget::setHeight(h);
	m_img.resize(m_img.getWidth(), h);
}

void ska::ImageRectangle::render(const Renderer& renderer) const {
	if(!isVisible()) {
		return;
	}
	const auto& pos = getAbsolutePosition();
	renderer.render(m_img, pos.x, pos.y, nullptr);
}

