#include "Exceptions/IllegalArgumentException.h"
#include "Exceptions/ExceptionTrigger.h"
#include "SDLRenderer.h"
#include "Logging/Logger.h"
#include "SDLWindow.h"
#include "AnimatedTexture.h"
#include "SDL_RectConverter.h"
#include <cassert>
#include "SDL_PointConverter.h"
#include "SDLLibrary.h"

ska::SDLRenderer::SDLRenderer(SDLWindow& window, int index, Uint32 flags) :
    m_renderer(nullptr),
	m_window(window),
	m_index(index),
	m_flags(flags) {
}

void ska::SDLRenderer::setRenderColor(const ska::Color & c) {
	m_currentRenderColor = c;
	SDLLibrary::get().setRenderDrawColor(*m_renderer, c.r, c.g, c.b, c.a);
}

void ska::SDLRenderer::load(SDL_Window* window, int index, Uint32 flags) {
    if(m_renderer != nullptr) {
        free();
    }

    m_renderer = SDLLibrary::get().createRenderer(*window, index, flags);
	setRenderColor(ska::Color(0xFF, 0xFF, 0xFF, 0xFF));

    if(m_renderer == nullptr) {
		SLOG(LogLevel::Error) << "Erreur lors de la creation de la fenetre SDL (renderer) : " << SDLLibrary::get().getError();
        ExceptionTrigger<IllegalArgumentException>("Bad instanciation : renderer cannot be null");
    }
}

SDL_Renderer * ska::SDLRenderer::unwrap() const {
	return m_renderer;
}

void ska::SDLRenderer::renderClear() const {
	SDLLibrary::get().renderClear(*m_renderer);
}

void ska::SDLRenderer::renderPresent() const {
	SDLLibrary::get().renderPresent(*m_renderer);
}

SDL_Texture* ska::SDLRenderer::createTextureFromSurface(const SDLSurface& s) const {
    return SDLLibrary::get().createTextureFromSurface(*m_renderer, *s.m_surface);
}

void ska::SDLRenderer::drawColorPoint(const Color& c, const Point<int>& pos) const {
	SDLLibrary::get().setRenderDrawColor(*m_renderer, c.r, c.g, c.b, c.a);
	SDLLibrary::get().renderDrawPoint(*m_renderer, pos.x, pos.y);
	SDLLibrary::get().setRenderDrawColor(*m_renderer, m_currentRenderColor.r, m_currentRenderColor.g, m_currentRenderColor.b, m_currentRenderColor.a);
}

void ska::SDLRenderer::drawColorRect(const Color& c, const Rectangle& r) const {
	SDLLibrary::get().setRenderDrawColor(*m_renderer, c.r, c.g, c.b, c.a);
	auto rNative = ToSDL_Rect(r);
	SDLLibrary::get().renderFillRect(*m_renderer, &rNative);
	SDLLibrary::get().setRenderDrawColor(*m_renderer, m_currentRenderColor.r, m_currentRenderColor.g, m_currentRenderColor.b, m_currentRenderColor.a);
}

void ska::SDLRenderer::drawColorLine(const Color& c, const Point<int>& p1, const Point<int>& p2) const {
	SDLLibrary::get().setRenderDrawColor(*m_renderer, c.r, c.g, c.b, c.a);
	SDLLibrary::get().renderDrawLine(*m_renderer, p1.x, p1.y, p2.x, p2.y);
	SDLLibrary::get().setRenderDrawColor(*m_renderer, m_currentRenderColor.r, m_currentRenderColor.g, m_currentRenderColor.b, m_currentRenderColor.a);
}

void ska::SDLRenderer::free() {
	SDLLibrary::get().destroyRenderer(m_renderer);
	m_renderer = nullptr;
}

void ska::SDLRenderer::update() const {
	assert(m_renderer != nullptr && "Renderer is not loaded");
	renderPresent();
	renderClear();
}

void ska::SDLRenderer::load() {
	load(m_window.getInstance(), m_index, m_flags);
}

SDL_Texture* ska::SDLRenderer::createTextureTarget(const unsigned int width, const unsigned int height) const {
	assert(m_renderer != nullptr && "Renderer is not loaded");
	return ska::SDLLibrary::get().createTexture(*m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
}

void ska::SDLRenderer::setRenderDrawBlendMode(int blendMode) {
	assert(m_renderer != nullptr && "Renderer is not loaded");
	ska::SDLLibrary::get().setRenderDrawBlendMode(*m_renderer, static_cast<SDL_BlendMode>(blendMode));
}

void ska::SDLRenderer::setRenderTarget(Texture* t) {
	assert(m_renderer != nullptr && "Renderer is not loaded");
	if (t != nullptr && t->getInstance() != nullptr) {
		t->getInstance()->asTarget(*m_renderer);
	} else {
		ska::SDLLibrary::get().setRenderTarget(*m_renderer, nullptr);
	}
}

void ska::SDLRenderer::render(const Texture& t, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const {
	auto instance = t.getInstance();
	if (instance != nullptr) {
		instance->load(*this);

		auto destBuf = SDL_Rect{ posX, posY, static_cast<int>(t.getWidth()), static_cast<int>(t.getHeight()) };

		if (clip != nullptr) {
			destBuf.w = clip->w;
			destBuf.h = clip->h;
		}

		SDL_Rect rClip{};
		if (clip != nullptr) { rClip = ToSDL_Rect(*clip); }
		SDL_Point pRotationCenter;
		if(rotationCenter != nullptr) { pRotationCenter = ToSDL_Point(*rotationCenter); }
		assert(SDLLibrary::get().renderCopyEx(*m_renderer, *instance->m_texture, clip != nullptr ? &rClip : nullptr, &destBuf, angle, rotationCenter != nullptr ? &pRotationCenter : nullptr, SDL_FLIP_NONE) == 0);
	}
}

void ska::SDLRenderer::render(const AnimatedTexture& at, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const {
	at.refresh();

	auto tmp = at.m_anim.getCurrentFrame();

	if(at.m_gifMode) {
		at.m_gif.refresh();

		tmp.x = posX + at.m_relativePos.x;
		tmp.y = posY + at.m_relativePos.y;

		auto rTmp = ToSDL_Rect(tmp);
		SDL_Rect rClip;
		if (clip != nullptr) { rClip = ToSDL_Rect(*clip); }
		SDLLibrary::get().renderCopy(*m_renderer, *at.m_gif.m_actTexture, clip != nullptr ? &rClip : nullptr, &rTmp);
	} else {
		render(at.m_sprite, posX + at.m_relativePos.x, posY + at.m_relativePos.y, &tmp, angle, rotationCenter);
	}


}

ska::SDLRenderer::~SDLRenderer() {
    free();
}
