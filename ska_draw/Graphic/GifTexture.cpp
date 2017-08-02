#include <CEV_gif.h>
#include "GifTexture.h"
#include "Exceptions/IllegalStateException.h"
#include "Draw/SDLRenderer.h"
#include "Exceptions/FileException.h"
#include <iostream>

ska::SDLRenderer* ska::GifTexture::m_renderer = nullptr;

ska::GifTexture::GifTexture()
	: m_animation(nullptr),
	  m_actTexture(nullptr), 
	  m_width(0), 
	  m_height(0) { }

ska::GifTexture::~GifTexture() {
	free();
}

void ska::GifTexture::setDelay(unsigned delay) const {
	CEV_gifTimeSet(m_animation, GIF_ALL, delay);
}

void ska::GifTexture::stop(bool x) const {
	setLoopMode(x ? GIF_STOP : GIF_REPEAT_FOR);
}

void ska::GifTexture::setLoopMode(unsigned int loopMode) const {
	CEV_gifLoopMode(m_animation, loopMode);
}

void ska::GifTexture::load(const std::string& spriteName) {
	checkRenderer();
	free();
	m_animation = CEV_gifAnimLoad(spriteName.c_str(), m_renderer->unwrap());
	
	if(m_animation == nullptr) {
		throw ska::FileException("unable to load gif file : " + spriteName);
	}

	m_actTexture = CEV_gifTexture(m_animation);
	if(m_actTexture == nullptr) {
		CEV_gifAnimFree(m_animation);
		m_animation = nullptr;
		throw ska::FileException("unable to create texture from gif file : " + spriteName);
	}

	int w, h;
	if (SDL_QueryTexture(m_actTexture, nullptr, nullptr, &w, &h)) {
		m_width = static_cast<unsigned int>(w);
		m_height = static_cast<unsigned int>(h);
	} else {
		std::cerr << "unable to query texture : file " << spriteName << std::endl;
	}

	stop(false);
}

unsigned int ska::GifTexture::getWidth() const {
	return m_width;
}

unsigned int ska::GifTexture::getHeight() const {
	return m_height;
}

int ska::GifTexture::render(int x, int y, const Rectangle* clip) const{
	
	checkRenderer();
	CEV_gifAnimAuto(m_animation);

	Rectangle destBuf = { x, y, m_width, m_height };
	if (clip != nullptr) {
		destBuf.w = clip->w;
		destBuf.h = clip->h;
	}

	return SDL_RenderCopy(m_renderer->unwrap(), m_actTexture, clip, &destBuf);
}

void ska::GifTexture::reset() const {
	CEV_gifLoopReset(m_animation);
}

void ska::GifTexture::nextFrame() const{
	CEV_gifFrameNext(m_animation);
}

void ska::GifTexture::free() {
	CEV_gifAnimFree(m_animation);
	m_animation = nullptr;
	SDL_DestroyTexture(m_actTexture);
	m_actTexture = nullptr;
}

void ska::GifTexture::checkRenderer() {
	if (m_renderer == nullptr) {
		m_renderer = SDLRenderer::getDefaultRenderer();
		if (m_renderer == nullptr) {
			throw IllegalStateException("The Texture's default window is not set");
		}
	}
}
