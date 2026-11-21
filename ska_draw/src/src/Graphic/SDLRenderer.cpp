#include "Graphic/__internalConfig/LoggerConfig.h"
#include "Core/Exceptions/IllegalArgumentException.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "SDLRenderer.h"
#include "Logging/Logger.h"
#include "SDLWindow.h"
#include "AnimatedTexture.h"
#include "SDL_RectConverter.h"
#include <cassert>
#include <cstdlib>
#include "SDL_PointConverter.h"
#include "Core/SDLLibrary.h"

ska::SDLRenderer::SDLRenderer(SDLWindow& window) :
    m_renderer(nullptr),
	m_window(window) {
}

void ska::SDLRenderer::setRenderColor(const ska::Color & c) {
	m_currentRenderColor = c;
	SDLLibrary::get().setRenderDrawColor(*m_renderer, c.r, c.g, c.b, c.a);
}

void ska::SDLRenderer::load(SDL_Window* window) {
    if(m_renderer != nullptr) {
        free();
    }

	const auto& sdl = SDLLibrary::get();
	// The "gpu" driver runs the shaders : chosen first, unless SDL_RENDER_DRIVER asks for another one
	if (std::getenv("SDL_RENDER_DRIVER") == nullptr) {
		m_renderer = sdl.createRenderer(*window, "gpu");
		if (m_renderer == nullptr) {
			SLOG(LogLevel::Warn) << "No \"gpu\" renderer (" << sdl.getError() << ") : the shaders cannot be used";
		}
	}
	if (m_renderer == nullptr) {
		m_renderer = sdl.createRenderer(*window, nullptr);
	}

    if(m_renderer == nullptr) {
		SLOG(LogLevel::Error) << "Erreur lors de la creation de la fenetre SDL (renderer) : " << sdl.getError();
        ExceptionTrigger<IllegalArgumentException>("Bad instanciation : renderer cannot be null");
    }
	SLOG(LogLevel::Info) << "Renderer : " << sdl.rendererName(*m_renderer);
	setRenderColor(ska::Color(0xFF, 0xFF, 0xFF, 0xFF));
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
	load(m_window.getInstance());
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
		if (m_effect == nullptr) {
			SDLLibrary::get().renderCopyEx(*m_renderer, *instance->m_texture, clip != nullptr ? &rClip : nullptr, &destBuf, angle, rotationCenter != nullptr ? &pRotationCenter : nullptr, SDL_FLIP_NONE);
			return;
		}

		// An effect on this drawing only : the texture is shared, its modes are restored after
		const auto& effect = *m_effect;
		auto* texture = instance->m_texture;
		const auto width = static_cast<int>(destBuf.w * effect.scaleX + 0.5F);
		const auto height = static_cast<int>(destBuf.h * effect.scaleY + 0.5F);
		destBuf.x += (destBuf.w - width) / 2;
		destBuf.y += effect.anchorBottom ? destBuf.h - height : (destBuf.h - height) / 2;
		destBuf.w = width;
		destBuf.h = height;
		destBuf.x += effect.offsetX;
		destBuf.y += effect.offsetY;

		const auto& sdl = SDLLibrary::get();
		auto clipPointer = clip != nullptr ? &rClip : nullptr;
		auto centerPointer = rotationCenter != nullptr ? &pRotationCenter : nullptr;
		Uint8 red = 255, green = 255, blue = 255, alpha = 255;
		auto blendMode = SDL_BLENDMODE_BLEND;
		sdl.getTextureColorMod(*texture, &red, &green, &blue);
		sdl.getTextureAlphaMod(*texture, &alpha);
		sdl.getTextureBlendMode(*texture, &blendMode);
		sdl.setTextureColorMod(*texture, static_cast<Uint8>(red * effect.red / 255), static_cast<Uint8>(green * effect.green / 255), static_cast<Uint8>(blue * effect.blue / 255));
		sdl.setTextureAlphaMod(*texture, static_cast<Uint8>(alpha * effect.alpha / 255));
		if (blendMode == SDL_BLENDMODE_NONE) {
			sdl.setTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);
		}
		sdl.renderCopyEx(*m_renderer, *texture, clipPointer, &destBuf, angle, centerPointer, SDL_FLIP_NONE);
		if (effect.flash > 0) {
			// Added twice over the drawing : its colors go to white
			sdl.setTextureBlendMode(*texture, SDL_BLENDMODE_ADD);
			sdl.setTextureColorMod(*texture, 255, 255, 255);
			sdl.setTextureAlphaMod(*texture, static_cast<Uint8>(effect.flash * effect.alpha / 255));
			sdl.renderCopyEx(*m_renderer, *texture, clipPointer, &destBuf, angle, centerPointer, SDL_FLIP_NONE);
			sdl.renderCopyEx(*m_renderer, *texture, clipPointer, &destBuf, angle, centerPointer, SDL_FLIP_NONE);
		}
		sdl.setTextureColorMod(*texture, red, green, blue);
		sdl.setTextureAlphaMod(*texture, alpha);
		sdl.setTextureBlendMode(*texture, blendMode);
	}
}

void ska::SDLRenderer::render(const AnimatedTexture& at, int posX, int posY, Rectangle const* clip, double angle, Point<int> const* rotationCenter) const {
	at.refresh();

	auto tmp = at.m_anim.getCurrentFrame();

	render(at.m_sprite, posX + at.m_relativePos.x, posY + at.m_relativePos.y, &tmp, angle, rotationCenter);


}

ska::SDLRenderer::~SDLRenderer() {
    free();
}
