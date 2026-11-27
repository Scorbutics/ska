#include <limits>
#include "Image.h"
#include "Core/Draw/Renderer.h"

ska::Image::Image(WidgetPanel& parent, const std::string& imagePath, Point<int> relativePos, bool alpha, Rectangle* clip) :
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
	resizeOwn(w, m_img.getHeight());
}

void ska::Image::setHeight(unsigned int h) {
	Widget::setHeight(h);
	if (m_clip.has_value()) {
		m_clip->h = h;
	}
	resizeOwn(m_img.getWidth(), h);
}

void ska::Image::resizeOwn(unsigned int w, unsigned int h) {
	if (m_img.getWidth() == w && m_img.getHeight() == h) {
		return;
	}
	// The texture of an image is shared by every texture of the same file (Texture : ManagedResource) : resized, it is
	// its own (else every sprite of the file, created after, would have this size : ex a Pokemon of the team shown in a
	// slot of 64 x 64, every Pokemon of its kind on the map drawn from frames of 10 x 8 pixels, unseen)
	m_img.lifetimeSeparation();
	m_img.resize(w, h);
}

void ska::Image::render(Renderer& renderer) const {
	if(!isVisible()) {
		return;
	}
	const auto& pos = getAbsolutePosition();
	renderer.render(m_img, pos.x, pos.y, m_clip.has_value() ? &m_clip.value() : nullptr);
}

