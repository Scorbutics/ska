#include <CEV_gif.h>
#include "GifTexture.h"
#include "Exceptions/IllegalStateException.h"
#include "Draw/SDLRenderer.h"
#include "Exceptions/FileException.h"
#include "Logging/Logger.h"
#include <iostream>
#include "Core/CodeDebug/CodeDebug.h"

ska::SDLRenderer* ska::GifTexture::m_renderer = nullptr;

ska::GifTexture::GifTexture()
	: m_animation(NULL),
	  m_actTexture(NULL),
	  m_width(0),
	  m_height(0) { }

ska::GifTexture::~GifTexture() {
	free();
}

ska::GifTexture::GifTexture(const GifTexture& t) :
    m_animation(NULL),
	m_actTexture(NULL),
	m_width(0),
	m_height(0) {
    operator=(t);
}

ska::GifTexture& ska::GifTexture::operator=(const GifTexture& t) {
    m_spriteName = t.m_spriteName;
    if(!m_spriteName.empty()) {
        load(m_spriteName);
    }
    return *this;
}

void ska::GifTexture::setDelay(unsigned delay) const {
    if(m_animation != nullptr) {
        CEV_gifTimeSet(m_animation, GIF_ALL, delay);
    }
}

void ska::GifTexture::stop(bool x) const {
	setLoopMode(x ? GIF_STOP : GIF_REPEAT_FOR);
}

void ska::GifTexture::setLoopMode(unsigned int loopMode) const {
    if(m_animation != nullptr) {
        CEV_gifLoopMode(m_animation, loopMode);
    }
}

void ska::GifTexture::load(const std::string& spriteName) {
	checkRenderer();
	free();
	m_animation = CEV_gifAnimLoad(spriteName.c_str(), m_renderer->unwrap());

	if(m_animation == NULL) {
		throw ska::FileException("unable to load gif file : " + spriteName);
	}
    SKA_LOG_INFO("Loaded GIF file ", spriteName, " (0x", m_animation, ")");

	m_actTexture = CEV_gifTexture(m_animation);
	if(m_actTexture == NULL) {
		CEV_gifAnimFree(m_animation);
		m_animation = NULL;
		throw ska::FileException("unable to create texture from gif file : " + spriteName);
	}
    SKA_LOG_INFO("Loaded Texture GIF file ", spriteName, " (0x", m_actTexture, ")");

	int w, h;
	if (!SDL_QueryTexture(m_actTexture, NULL, NULL, &w, &h)) {
		m_width = static_cast<unsigned int>(w);
		m_height = static_cast<unsigned int>(h);
	} else {
		std::cerr << "unable to query texture : file " << spriteName << " : " << SDL_GetError() << std::endl;
	}

    m_spriteName = spriteName;
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
    if(m_animation != nullptr) {
        CEV_gifLoopReset(m_animation);
    }
}

void ska::GifTexture::nextFrame() const{
	CEV_gifFrameNext(m_animation);
}

void ska::GifTexture::free() {
	SKA_DBG_ONLY(
	if (m_animation != nullptr) {
		SKA_LOG_INFO("Freeing GIF file ", m_spriteName, " (0x", m_animation, ")");
	});
	CEV_gifAnimFree(m_animation);
	m_animation = NULL;
}

void ska::GifTexture::checkRenderer() {
	if (m_renderer == nullptr) {
		m_renderer = SDLRenderer::getDefaultRenderer();
		if (m_renderer == nullptr) {
			throw IllegalStateException("The Texture's default window is not set");
		}
	}
}
