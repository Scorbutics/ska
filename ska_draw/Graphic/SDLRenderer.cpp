#include "Exceptions/IllegalArgumentException.h"
#include "SDLRenderer.h"
#include "Logging/Logger.h"
#include "SDLWindow.h"
#include "AnimatedTexture.h"
#include "SDL_RectConverter.h"
#include <cassert>

ska::SDLRenderer::SDLRenderer(SDLWindow& window, int index, Uint32 flags) :
    m_renderer(nullptr),
	m_window(window),
	m_index(index),
	m_flags(flags) {
}

void ska::SDLRenderer::setRenderColor(const ska::Color & c) {
	m_currentRenderColor = c;
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
}

void ska::SDLRenderer::load(SDL_Window* window, int index, Uint32 flags) {
    if(m_renderer != nullptr) {
        free();
    }

    m_renderer = SDL_CreateRenderer(window, index, flags);
	setRenderColor(ska::Color(0xFF, 0xFF, 0xFF, 0xFF));

    if(m_renderer == nullptr) {
		SKA_LOG_ERROR("Erreur lors de la création de la fenêtre SDL (renderer) :", SDL_GetError());
        throw IllegalArgumentException("Bad instanciation : renderer cannot be null");
    }
}

SDL_Renderer * ska::SDLRenderer::unwrap() const {
	return m_renderer;
}

void ska::SDLRenderer::renderClear() const {
    SDL_RenderClear(m_renderer);
}

void ska::SDLRenderer::renderPresent() const {
    SDL_RenderPresent(m_renderer);
}

SDL_Texture* ska::SDLRenderer::createTextureFromSurface(const SDLSurface& s) const {
    return SDL_CreateTextureFromSurface(m_renderer, s.m_surface);
}

void ska::SDLRenderer::drawColorPoint(const Color& c, const Point<int>& pos) const {
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderDrawPoint(m_renderer, pos.x, pos.y);
	SDL_SetRenderDrawColor(m_renderer, m_currentRenderColor.r, m_currentRenderColor.g, m_currentRenderColor.b, m_currentRenderColor.a);
}

void ska::SDLRenderer::drawColorRect(const Color& c, const Rectangle& r) const {
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
	auto rNative = ToSDL_Rect(r);
	SDL_RenderFillRect(m_renderer, &rNative);
	SDL_SetRenderDrawColor(m_renderer, m_currentRenderColor.r, m_currentRenderColor.g, m_currentRenderColor.b, m_currentRenderColor.a);
}

void ska::SDLRenderer::drawColorLine(const Color& c, const Point<int>& p1, const Point<int>& p2) const {
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
	SDL_SetRenderDrawColor(m_renderer, m_currentRenderColor.r, m_currentRenderColor.g, m_currentRenderColor.b, m_currentRenderColor.a);
}

void ska::SDLRenderer::free() {
    SDL_DestroyRenderer(m_renderer);
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

void ska::SDLRenderer::render(const Texture& t, int posX, int posY, Rectangle const* clip) const {
	auto instance = t.getInstance();
	if (instance != nullptr) {
		instance->load(*this);

		SDL_Rect destBuf = { posX, posY, static_cast<int>(t.getWidth()), static_cast<int>(t.getHeight()) };

		if (clip != nullptr) {
			destBuf.w = clip->w;
			destBuf.h = clip->h;
		}

		SDL_Rect rClip;
		if (clip != nullptr) { rClip = ToSDL_Rect(*clip); }
		SDL_RenderCopy(m_renderer, instance->m_texture, clip != nullptr ? &rClip : nullptr, &destBuf);
	}
}

void ska::SDLRenderer::render(const AnimatedTexture& at, int posX, int posY, Rectangle const* clip) const {
	at.refresh();

	auto tmp = at.m_anim.getCurrentFrame();

	if(at.m_gifMode) {
		at.m_gif.refresh();

		tmp.x = posX + at.m_relativePos.x;
		tmp.y = posY + at.m_relativePos.y;

		auto rTmp = ToSDL_Rect(tmp);
		SDL_Rect rClip;
		if (clip != nullptr) { rClip = ToSDL_Rect(*clip); }
		SDL_RenderCopy(m_renderer, at.m_gif.m_actTexture, clip != nullptr ? &rClip : nullptr, &rTmp);
	} else {
		render(at.m_sprite, posX + at.m_relativePos.x, posY + at.m_relativePos.y, &tmp);
	}


}

ska::SDLRenderer::~SDLRenderer() {
    free();
}
