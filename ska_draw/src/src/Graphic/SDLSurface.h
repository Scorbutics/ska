#pragma once

#include <SDL.h>
#include <string>
#include "Core/Draw/Color.h"
#include "Base/Values/MovableNonCopyable.h"
#include "../../external/gsl/gsl/pointers"

namespace ska {

	class Font;

	/**
	* \brief SDL specific, a RAII wrapper of a SDL_Surface instance
	*/
	class SDLSurface : public MovableNonCopyable {
	    friend class SDLRenderer;

	public:
		SDLSurface();
		~SDLSurface() override;

		SDLSurface(SDLSurface&& surf) noexcept;
		SDLSurface& operator=(SDLSurface&& surf) noexcept;

		void load(const std::string& file, Color const* colorKey);
		void load32(const std::string& file);
		void loadFromText(const Font& font, const std::string& text, Color c);
		void loadFromColoredRect(const Color& color, const SDL_Rect& rect, const Color* outlineColor);

		Color getPixel32Color(int x, int y) const;
		uint32_t getPixel32(int x, int y) const;
		uint32_t getPixel32(int pixIndex) const;
		const SDL_PixelFormat * getFormat() const;

		SDL_Surface* getInstance() const;

	private:
		bool checkSurfaceValidity(const std::string& fileName, bool is32 = false);
		void free();

		uint8_t m_r {};
		uint8_t m_g {};
		uint8_t m_b {};
		uint8_t m_a {};

		gsl::owner<SDL_Surface*> m_surface {};

	};
}
