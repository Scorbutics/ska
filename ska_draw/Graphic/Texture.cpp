#include "Texture.h"
#include "Draw/Color.h"
#include "Draw/Renderer.h"
#include "Draw/TextureData.h"

ska::Texture::Texture(const std::string& id, int r, int g, int b, int a) : 
	ManagedResource() {
	load(id, r, g, b, a);
}

void ska::Texture::load(const std::string& id, int r, int g, int b, int a) {
	Color finalColor;
	finalColor.a = static_cast<Uint8>(a);
	finalColor.b = static_cast<Uint8>(b);
	finalColor.g = static_cast<Uint8>(g);
	finalColor.r = static_cast<Uint8>(r);

	loadFromKey(TextureData(id, finalColor, ska::Rectangle{}, SPRITE, 0));
}

void ska::Texture::setColor(Uint8 r, Uint8 g, Uint8 b) const {
	if (m_value != nullptr) {
		m_value->setColor(r, g, b);
	}
}

void ska::Texture::setBlendMode(int blending) const {
	if (m_value != nullptr) {
		m_value->setBlendMode(blending);
	}
}

void ska::Texture::setAlpha(Uint8 alpha) const{
	if (m_value != nullptr) {
		m_value->setBlendMode(alpha);
	}
}

void ska::Texture::resize(unsigned int width, unsigned int height) {
	if (m_value != nullptr) {
		m_value->resize(width, height);
	}
}

void ska::Texture::loadFromText(unsigned int fontSize, std::string text, Color c) {
	loadFromKey(TextureData(text, c, ska::Rectangle{}, TEXT, fontSize));
}

void ska::Texture::loadFromColoredRect(unsigned int width, unsigned int height, Color c) {
	loadFromKey(TextureData("", c, { 0, 0, static_cast<int>(width), static_cast<int>(height) }, RECT, 0));
	lifetimeSeparation();
	resize(width, height);
}

unsigned int ska::Texture::getWidth() const {
	return m_value == nullptr ? 0 : m_value->getWidth();
}

unsigned int ska::Texture::getHeight() const {
	return m_value == nullptr ? 0 : m_value->getHeight();
}

ska::SDLTexture* ska::Texture::getInstance() const {
	return m_value.get();
}

void ska::Texture::render(const Renderer& renderer, int posX, int posY, const Rectangle* clip) const {
	renderer.render(*this, posX, posY, clip);
}



