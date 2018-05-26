#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cassert>
#include "SDLLibrary.h"
#include "Font.h"
#include "SDLSurface.h"
#include "Logging/Logger.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "../GraphicModule.h"
#include "../SDLTTFLibrary.h"
#include "../SDLImageLibrary.h"

SDL_Color ColorToNative(const ska::Color& c){
	return SDL_Color{ c.r, c.g, c.b, c.a };
}

ska::SDLSurface::SDLSurface() : m_a(255) {
	m_surface = nullptr;
}

const SDL_PixelFormat * ska::SDLSurface::getFormat() const {
	return m_surface == nullptr ? nullptr : m_surface->format;
}

SDL_Surface* ska::SDLSurface::getInstance() const {
	return m_surface;
}

void ska::SDLSurface::loadFromText(const Font& font, const std::string& text, Color c) {
	free();
	m_surface = SDLTTFLibrary::get().renderTextBlended(*font.getInstance(), text.c_str(), ColorToNative(c));

	if (!checkSurfaceValidity("(Text) : " + text)) {
		return;
	}

	c.fill(m_r, m_g, m_b, &m_a);
}

void ska::SDLSurface::loadFromColoredRect(const Color& color, const SDL_Rect& rect) {
	free();
	m_surface = SDLLibrary::get().createRGBSurface(0, rect.w, rect.h, 32, 0, 0, 0, 0);

	if (!checkSurfaceValidity("(Rectangle)")) {
		return;
	}

	SDL_Rect sRect = { 0, 0, rect.w, rect.h };
	color.fill(m_r, m_g, m_b, &m_a);
	SDLLibrary::get().fillRect(*m_surface, &sRect, SDLLibrary::get().mapRGBA(*m_surface->format, color.r, color.g, color.b, color.a));
}

void ska::SDLSurface::load(const std::string& file, Color const* colorKey) {
	free();
	m_surface = SDLImageLibrary::get().load(file);

	if(!checkSurfaceValidity(file)) {
		return;
	}

	if (colorKey != nullptr) {
		colorKey->fill(m_r, m_g, m_b, &m_a);
		SDLLibrary::get().setColorKey(*m_surface, SDL_TRUE, SDLLibrary::get().mapRGBA(*m_surface->format, m_r, m_g, m_b, m_a));
	}

	if (m_a < 255) {
		SDLLibrary::get().setSurfaceAlphaMod(*m_surface, m_a);
	}
}

bool ska::SDLSurface::checkSurfaceValidity(const std::string& fileName, const bool is32) {
	if(fileName == std::string(NOSUCHFILE)) {
		return true;
	}

	SKA_DEBUG_ONLY(if (m_surface == nullptr) {
		SKA_LOG_ERROR("Erreur lors du chargement de l'image \"", fileName, "\" : ", SDLLibrary::get().getError());
	});

	if (m_surface == nullptr) {
        if(!is32) {
            load(NOSUCHFILE, nullptr);
        } else {
            return false;
        }
	}

	SKA_DEBUG_ONLY(if (m_surface == nullptr) {
		SKA_LOG_ERROR("Erreur du chargement de l'image " + fileName + ". Apres tentative de recuperation, impossible de charger l'image \"" NOSUCHFILE "\" : " + std::string(SDLLibrary::get().getError()));
	});

	return m_surface != nullptr;
}

void ska::SDLSurface::load32(const std::string& file) {
	free();
	m_surface = nullptr;
	const auto imageRam = SDLImageLibrary::get().load(file);
	if (imageRam != nullptr) {
		m_surface = SDLLibrary::get().createRGBSurface(0, imageRam->w, imageRam->h, 32, 0, 0, 0, 0);
		if (m_surface == nullptr) {
			goto loadImage32Free;
		}

		/* Copie l'image image_ram de moins de 32 bits vers image_result qui fait 32 bits */
		SDLLibrary::get().upperBlit(*imageRam, nullptr, *m_surface, nullptr);

		/* Clean up phase */
	loadImage32Free:
		SDLLibrary::get().freeSurface(imageRam);
	}

	checkSurfaceValidity(file);

}

ska::Color ska::SDLSurface::getPixel32Color(int x, int y) const {
	if (m_surface == nullptr) {
		return Color{};
	}

	Color c;
	const auto pix = getPixel32(x, y);
	SDLLibrary::get().getRGB(pix, *getFormat(), &c.r, &c.g, &c.b);
	return c;
}

uint32_t ska::SDLSurface::getPixel32(int x, int y) const {
	if (m_surface == nullptr || x < 0 || x > m_surface->w - 1 || y < 0 || y > m_surface->h - 1) {
        SKA_LOG_ERROR("Tentative d'acces a une zone hors limites sur l'image de dimensions ", m_surface->w, " par ", m_surface->h, " au coordonnees : (", x, "; ", y, ")");
		return 0;
	}
	return static_cast<uint32_t*>(m_surface->pixels)[y * (m_surface->pitch / 4) + x];
}

uint32_t ska::SDLSurface::getPixel32(int pixIndex) const {
	return getPixel32(pixIndex % m_surface->w, pixIndex / m_surface->w);
}

void ska::SDLSurface::free() {
	SDLLibrary::get().freeSurface(m_surface);
	m_surface = nullptr;
	m_r = m_g = m_b = 0;
	m_a = 255;
}

ska::SDLSurface::~SDLSurface() {
	free();
}

ska::SDLSurface::SDLSurface(SDLSurface&& surf) noexcept {
	*this = std::move(surf);
}

ska::SDLSurface& ska::SDLSurface::operator=(SDLSurface&& surf) noexcept {
	std::swap(m_r, surf.m_r);
	std::swap(m_g, surf.m_g);
	std::swap(m_b, surf.m_b);
	std::swap(m_a, surf.m_a);

	m_surface = surf.m_surface;
	surf.m_surface = nullptr;
	return *this;
}
