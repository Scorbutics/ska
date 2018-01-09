#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Font.h"
#include "SDLSurface.h"
#include "Exceptions/FileException.h"
#include "Logging/Logger.h"
#include "Utils/SkaConstants.h"
#include "Core/CodeDebug/CodeDebug.h"

ska::SDLSurface::SDLSurface(): m_r(0), m_g(0), m_b(0), m_a(255) {
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
	m_surface = TTF_RenderText_Blended(font.getInstance(), text.c_str(), c.toNative());

	if (!checkSurfaceValidity("(Text) : " + text)) {
		return;
	}

	c.fill(m_r, m_g, m_b, &m_a);
}

void ska::SDLSurface::loadFromColoredRect(const Color& color, const SDL_Rect& rect) {
	free();
	m_surface = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0, 0, 0, 0);

	if (!checkSurfaceValidity("(Rectangle)")) {
		return;
	}

	SDL_Rect sRect = { 0, 0, rect.w, rect.h };
	color.fill(m_r, m_g, m_b, &m_a);
	SDL_FillRect(m_surface, &sRect, SDL_MapRGBA(m_surface->format, color.r, color.g, color.b, color.a));
}

void ska::SDLSurface::load(const std::string& file, Color const* colorKey) {
	free();
	m_surface = IMG_Load(file.c_str());

	if(!checkSurfaceValidity(file)) {
		return;
	}

	if (colorKey != nullptr) {
		colorKey->fill(m_r, m_g, m_b, &m_a);
		SDL_SetColorKey(m_surface, SDL_TRUE, SDL_MapRGBA(m_surface->format, m_r, m_g, m_b, m_a));
	}

	if (m_a < 255) {
		SDL_SetSurfaceAlphaMod(m_surface, m_a);
	}
}

bool ska::SDLSurface::checkSurfaceValidity(const std::string& fileName) {
	if(fileName == std::string(NOSUCHFILE)) {
		return true;
	}

	SKA_DBG_ONLY(if (m_surface == nullptr) {
		SKA_LOG_ERROR("Erreur lors du chargement de l'image \"", fileName, "\" : ", SDL_GetError());
	});

	if (m_surface == nullptr) {
		load(NOSUCHFILE, nullptr);
	}

	SKA_DBG_ONLY(if (m_surface == nullptr) {
		SKA_LOG_ERROR("Erreur du chargement de l'image " + fileName + ". Après tentative de récupération, impossible de charger l'image \"" NOSUCHFILE "\" : " + std::string(SDL_GetError()));
	});

	return m_surface != nullptr;
}

void ska::SDLSurface::load32(const std::string& file) {
	free();
	m_surface = nullptr;
	const auto imageRam = IMG_Load(file.c_str());
	if (imageRam != nullptr) {
		m_surface = SDL_CreateRGBSurface(0, imageRam->w, imageRam->h, 32, 0, 0, 0, 0);
		if (m_surface == nullptr) {
			goto loadImage32Free;
		}

		/* Copie l'image image_ram de moins de 32 bits vers image_result qui fait 32 bits */
		SDL_UpperBlit(imageRam, nullptr, m_surface, nullptr);

		/* Clean up phase */
	loadImage32Free:
		SDL_FreeSurface(imageRam);
	}

	checkSurfaceValidity(file);

}

ska::Color ska::SDLSurface::getPixel32Color(int x, int y) const {
	if (m_surface == nullptr) {
		return Color( 0, 0, 0, 0 );
	}

	Color c;
	c.a = 0;
	const auto pix = getPixel32(x, y);
	SDL_GetRGB(pix, getFormat(), &c.r, &c.g, &c.b);
	return c;
}

Uint32 ska::SDLSurface::getPixel32(int x, int y) const {
	if (m_surface == nullptr || x < 0 || x > m_surface->w - 1 || y < 0 || y > m_surface->h - 1) {
		return 0;
	}
	return static_cast<Uint32*>(m_surface->pixels)[y*(m_surface->pitch / 4) + x];
}


void ska::SDLSurface::free() {
	SDL_FreeSurface(m_surface);
	m_surface = nullptr;
	m_r = m_g = m_b = 0;
	m_a = 255;
}

ska::SDLSurface::~SDLSurface() {
	free();
}
