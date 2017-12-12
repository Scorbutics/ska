#include "AnimatedTexture.h"
#include "Exceptions/IllegalStateException.h"
#include "Draw/Renderer.h"

ska::AnimatedTexture::AnimatedTexture()
	: m_gifMode(false) {
	m_anim.setDelay(300);
}

void ska::AnimatedTexture::loadFromText(unsigned fontSize, const std::string& text, ska::Color c, unsigned horizontalFrames, unsigned verticalFrames, unsigned animatedFrames, bool isVertical) {
	m_gifMode = false;
	m_sprite.loadFromText(fontSize, text, c);
	recalculateFrames(horizontalFrames, verticalFrames, animatedFrames, isVertical);
}

void ska::AnimatedTexture::load(const std::string& spritePath, unsigned int horizontalFrames, unsigned int verticalFrames, unsigned int animatedFrames, bool isVertical, int r, int g, int b, int a) {
	if (spritePath.find(".gif") != std::string::npos) {
		m_gifMode = true;
		m_gif.load(spritePath);
	} else {
		m_gifMode = false;
		m_sprite.load(spritePath, r, g, b, a);
	}
	recalculateFrames(horizontalFrames, verticalFrames, animatedFrames, isVertical);
}

void ska::AnimatedTexture::recalculateFrames(unsigned int horizontalFrames, unsigned int verticalFrames, unsigned int animatedFrames, const bool isVertical) {
	m_anim.setFrames(animatedFrames);
	m_anim.setVertical(isVertical);
	const auto width = (m_gifMode ? m_gif.getWidth() : m_sprite.getWidth()) / horizontalFrames;
	const auto height = (m_gifMode ? m_gif.getHeight() : m_sprite.getHeight()) / verticalFrames;
	Rectangle frame = { 0, 0, static_cast<int>(width), static_cast<int>(height) };
	m_anim.setOffsetAndFrameSize(frame);
}

ska::Rectangle ska::AnimatedTexture::getOffsetBase() const {
	return m_anim.getOffsetBase();
}

void ska::AnimatedTexture::setRelativePosition(const Point<int>& p) {
	m_relativePos = p;
}

void ska::AnimatedTexture::setOffset(const Point<int>& offset) {
	Rectangle tmp = m_anim.getCurrentFrame();
	tmp.x = offset.x;
	tmp.y = offset.y;
	m_anim.setOffsetAndFrameSize(tmp);
}

void ska::AnimatedTexture::operator=(const AnimatedTexture& text) {
	m_anim = text.m_anim;
	m_sprite = text.m_sprite;
	m_gifMode = text.m_gifMode;
}

void ska::AnimatedTexture::switchToFrame(unsigned int index) {
	if(m_gifMode) {
		throw ska::IllegalStateException("unimplemented");
	}
	
	m_anim.switchToFrame(index);	
}

void ska::AnimatedTexture::setBlendMode(int blendMode) {
	m_sprite.setBlendMode(blendMode);
}

void ska::AnimatedTexture::lifetimeSeparation() {
	m_sprite.lifetimeSeparation();
}

void ska::AnimatedTexture::refresh() const {
	if(m_gifMode) {
		m_gif.refresh();
	}
}

void ska::AnimatedTexture::setAlpha(int alpha) {
	m_sprite.setAlpha(static_cast<Uint8>(alpha));
}

void ska::AnimatedTexture::setColor(int red, int green, int blue) {
	m_sprite.setColor(static_cast<Uint8>(red), static_cast<Uint8>(green), static_cast<Uint8>(blue));
}

void ska::AnimatedTexture::setDelay(unsigned int delay) {
	m_anim.setDelay(delay);
	m_gif.setDelay(delay);
}

void ska::AnimatedTexture::update() {
	m_anim.updateFrame();
}

unsigned int ska::AnimatedTexture::getWidth() const {
	return m_gifMode ? m_gif.getWidth() : m_anim.getCurrentFrame().w;
}

unsigned int ska::AnimatedTexture::getHeight() const {
	return m_gifMode ? m_gif.getHeight() : m_anim.getCurrentFrame().h;
}

unsigned int ska::AnimatedTexture::getFullWidth() const {
	return m_gifMode ? m_gif.getWidth() : m_sprite.getWidth();
}

unsigned int ska::AnimatedTexture::getFullHeight() const {
	return m_gifMode ? m_gif.getHeight() : m_sprite.getHeight();
}

void ska::AnimatedTexture::stop(const bool x) {
	m_anim.stop(x);
	m_gif.stop(x);
}

void ska::AnimatedTexture::reset() {
	m_anim.switchToFrame(1);
	m_gif.reset();
}

void ska::AnimatedTexture::nextFrame() {
	if (m_gifMode) {
		m_gif.nextFrame();
	} else {
		m_anim.nextFrame();
	}
	
}

void ska::AnimatedTexture::render(const Renderer& renderer, int posX, int posY, const Rectangle* clip) const {
	renderer.render(*this, posX, posY, clip);
}
