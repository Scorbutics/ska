#include <limits>
#include "Image.h"
#include "Core/Draw/Renderer.h"

ska::Image::Image(Widget& parent, const std::string& imagePath, Point<int> relativePos, bool alpha, Rectangle* clip) :
	Widget(parent, relativePos),
	m_img(imagePath, Texture::DEFAULT_T_RED, Texture::DEFAULT_T_GREEN, Texture::DEFAULT_T_BLUE, alpha ? 128 : -1) {
	replaceWith(std::move(m_img), clip);
}

void ska::Image::replaceWith(ska::Texture img, Rectangle* clip) {
	m_img = img;

	if (clip != nullptr) {
		m_clip = *clip;

		Image::setWidth(m_clip->w);
		Image::setHeight(m_clip->h);
	}
	else {
		Image::setWidth(m_img.getWidth());
		Image::setHeight(m_img.getHeight());
	}
}

void ska::Image::setWidth(unsigned int w) {
	Widget::setWidth(w);
	if (m_clip.has_value()) {
		m_clip->w = w;
	}
	m_img.resize(w, m_img.getHeight());
}

void ska::Image::setHeight(unsigned int h) {
	Widget::setHeight(h);
	if (m_clip.has_value()) {
		m_clip->h = h;
	}
	m_img.resize(m_img.getWidth(), h);
}

void ska::Image::render(Renderer& renderer) const {
	if(!isVisible()) {
		return;
	}
	const auto& pos = getAbsolutePosition();
	renderer.render(m_img, pos.x, pos.y, m_clip.has_value() ? &m_clip.value() : nullptr);
}

