#pragma once

#include <map>
#include <SDL_ttf.h>

namespace ska {
	/**
	 * \brief SDL specific, a RAII wrapper of a TTF_Font instance
	 */
	class SDLFont {
	public:
		SDLFont();
		
		SDLFont(SDLFont&) = delete;
		SDLFont(SDLFont&&) = default;
		SDLFont& operator=(const SDLFont&) = delete;
		SDLFont& operator=(SDLFont&&) = default;

		explicit SDLFont(unsigned int fontSize);
		~SDLFont();

		TTF_Font* getInstance();

		static std::string DEFAULT_FONT_FILE;

	private:
		void open(const std::string& fontFile, unsigned int fontSize);
		TTF_Font* m_fontC;


	};
}
