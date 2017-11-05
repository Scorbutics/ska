#pragma once

#include <SDL.h>
#include "Color.h"

namespace ska {
	class Font;

	/**
	* \brief SDL specific, a RAII wrapper of a SDL_Surface instance
	*/
	class SDLSurface {
	    friend class SDLRenderer;

	public:
		SDLSurface();

		SDLSurface(const SDLSurface&) = delete;
		SDLSurface& operator=(const SDLSurface&) = delete;

		~SDLSurface();

		void load(const std::string& file);
		void load32(const std::string& file);
		void loadFromText(const Font& font, const std::string& text, Color c);
		void loadFromColoredRect(const Color& color, const SDL_Rect& rect);

		Color getPixel32Color(int x, int y) const;
		Uint32 getPixel32(int x, int y) const;
		const SDL_PixelFormat * getFormat() const;

		SDL_Surface* getInstance() const;
		

	private:
		void free();
		SDL_Surface* m_surface;
	};
}
