#include <iostream>
#include "SDLRenderer.h"
#include "Logging/Logger.h"
#include "Core/Window.h"
#include "AnimatedTexture.h"

ska::SDLRenderer::SDLRenderer(Window& window, int index, Uint32 flags) :
    m_renderer(nullptr) {
	load(window.getInstance() , index, flags);
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
		SKA_LOG_ERROR("Erreur lors de la cr�ation de la fen�tre SDL (renderer) :", SDL_GetError());
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
	SDL_RenderFillRect(m_renderer, &r);
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
	renderPresent();
	renderClear();
}

void ska::SDLRenderer::render(const Texture& t, int posX, int posY, Rectangle const* clip) const {	
	auto instance = t.getInstance();
	if (instance != nullptr) {
		instance->load(*this);

		Rectangle destBuf = { posX, posY, t.getWidth(), t.getHeight() };

		if (clip != nullptr) {
			destBuf.w = clip->w;
			destBuf.h = clip->h;
		}

		SDL_RenderCopy(m_renderer, instance->m_texture, clip, &destBuf);
	}
}

void ska::SDLRenderer::render(AnimatedTexture& at, int posX, int posY, Rectangle const* clip) const {
	at.refresh();
	const auto& tmp = at.m_anim.getCurrentFrame();

	if(at.m_gifMode) {
		
		at.m_gif.refresh();

		Rectangle destBuf = { posX + at.m_relativePos.x, posX + at.m_relativePos.y, at.m_gif.getWidth(), at.m_gif.getHeight() };

		if (clip != nullptr) {
			destBuf.w = clip->w;
			destBuf.h = clip->h;
		}

		SDL_RenderCopy(m_renderer, at.m_gif.m_actTexture, clip, &tmp);
	} else {
		render(at.m_sprite, posX + at.m_relativePos.x, posY + at.m_relativePos.y, &tmp);
	}

	
}

ska::SDLRenderer::~SDLRenderer() {
    free();
}
