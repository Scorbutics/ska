#include "Texture.h"
#include "Draw/Color.h"
#include "Draw/Renderer.h"
#include "TextureData.h"

ska::Texture::Texture(const std::string& id, int r, int g, int b, int a) {
	load(id, r, g, b, a);
}

void ska::Texture::load(const std::string& id, int r, int g, int b, int a) {
	Color finalColor;
	finalColor.a = static_cast<Uint8>(a);
	finalColor.b = static_cast<Uint8>(b);
	finalColor.g = static_cast<Uint8>(g);
	finalColor.r = static_cast<Uint8>(r);

	loadFromKey(TextureData(id, finalColor, ska::Rectangle{}, EnumTextureType::SPRITE, 0));
}

void ska::Texture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	if (m_value != nullptr) {
		m_value->setColor(red, green, blue);
	}
}

void ska::Texture::setBlendMode(int blending) {
	if (m_value != nullptr) {
		m_value->setBlendMode(blending);
	}
}

void ska::Texture::setAlpha(Uint8 alpha) {
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
	loadFromKey(TextureData(text, c, ska::Rectangle{}, EnumTextureType::TEXT, fontSize));
}

void ska::Texture::loadFromColoredRect(unsigned int width, unsigned int height, Color c, const Color* outlineColor) {
	loadFromKey(TextureData("", c, { 0, 0, static_cast<int>(width), static_cast<int>(height) }, EnumTextureType::RECT, 0, outlineColor));
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

void ska::Texture::render(const Renderer& renderer, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const {
	renderer.render(*this, posX, posY, clip, angle, rotationCenter);
}



