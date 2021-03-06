#pragma once
#include "Base/DynamicLibrary/DynamicLibrary.h"
#include <SDL_ttf.h>
#include <gsl/pointers>

namespace ska {
	
		template<int Id>
		struct TTFIdNamedFunction {
			static constexpr auto id = Id;
			static std::string name;
		};

		enum TTFCalls {
			TTF_INIT,
			TTF_QUIT,
			TTF_OPEN_FONT,
			TTF_CLOSE_FONT,
			TTF_RENDER_TEXT_BLENDED
		};

	#define SKA_TTF_LIB_CALLS_DEFINE(ENUM, FUNCTION) \
			SKA_LIB_CALLS_DEFINE(TTFIdNamedFunction, ENUM, FUNCTION)

		SKA_TTF_LIB_CALLS_DEFINE(TTF_INIT, int());
		SKA_TTF_LIB_CALLS_DEFINE(TTF_QUIT, void());
		SKA_TTF_LIB_CALLS_DEFINE(TTF_OPEN_FONT, TTF_Font*(const char*, int));
		SKA_TTF_LIB_CALLS_DEFINE(TTF_CLOSE_FONT, void(TTF_Font*));
		SKA_TTF_LIB_CALLS_DEFINE(TTF_RENDER_TEXT_BLENDED, SDL_Surface*(TTF_Font *, const char *, SDL_Color));

		using SDLTFFDynLib = ska::DynamicLibrary <
			TTFIdNamedFunction<TTFCalls::TTF_INIT>,
			TTFIdNamedFunction<TTFCalls::TTF_QUIT>,
			TTFIdNamedFunction<TTFCalls::TTF_OPEN_FONT>,
			TTFIdNamedFunction<TTFCalls::TTF_CLOSE_FONT>,
			TTFIdNamedFunction<TTFCalls::TTF_RENDER_TEXT_BLENDED>
		>;
	

	class SDLTTFLibrary : public SDLTFFDynLib {
	public:
		int init() const {
			return call<TTFIdNamedFunction<TTF_INIT>>();
		}

		void quit() const {
			call<TTFIdNamedFunction<TTF_QUIT>>();
		}

		gsl::owner<TTF_Font*> openFont(const std::string& file, int ptsize) const {
			return call<TTFIdNamedFunction<TTF_OPEN_FONT>>(file.c_str(), std::move(ptsize));
		}

		void closeFont(gsl::owner<TTF_Font*> font) const {
			call<TTFIdNamedFunction<TTF_CLOSE_FONT>>(std::move(font));
		}

		gsl::owner<SDL_Surface*> renderTextBlended(TTF_Font& font, const std::string& text, SDL_Color color) const {
			return call<TTFIdNamedFunction<TTF_RENDER_TEXT_BLENDED>>(&font, text.c_str(), std::move(color));
		}
		
		static const SDLTTFLibrary& get();

	private:
		SDLTTFLibrary() : SDLTFFDynLib("SDL2_ttf") {}
	};
}
