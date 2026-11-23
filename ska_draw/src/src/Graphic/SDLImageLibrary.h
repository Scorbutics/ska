#pragma once
#include "Base/DynamicLibrary/DynamicLibrary.h"
#include <SDL3/SDL.h>
#include <gsl/pointers>

namespace ska {
	template<int Id>
	struct SDLImageIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	// The SDL3_image functions used by the engine, loaded at run time (libSDL3_image). It needs no initialization.
	enum SDLImageCalls {
		IMG_LOAD,
		IMG_SAVE_PNG
	};

#define SKA_IMG_LIB_CALLS_DEFINE(ENUM, FUNCTION) \
			SKA_LIB_CALLS_DEFINE(SDLImageIdNamedFunction, ENUM, FUNCTION)

	SKA_IMG_LIB_CALLS_DEFINE(IMG_LOAD, SDL_Surface* (const char *));
	SKA_IMG_LIB_CALLS_DEFINE(IMG_SAVE_PNG, bool (SDL_Surface*, const char *));

	using SDLImageDynLib = DynamicLibrary <
		SDLImageIdNamedFunction<IMG_LOAD>,
		SDLImageIdNamedFunction<IMG_SAVE_PNG>>;

	class SDLImageLibrary : public SDLImageDynLib {
		#define callSDLImage(enumIndex) call<SDLImageIdNamedFunction<enumIndex>>
	public:
		SDL_Surface* load(const std::string& file) const {
			return callSDLImage(IMG_LOAD)(file.c_str());
		}

		bool savePNG(SDL_Surface& surface, const std::string& file) const {
			return callSDLImage(IMG_SAVE_PNG)(&surface, file.c_str());
		}

		static const SDLImageLibrary& get();
		#undef callSDLImage

	private:
		SDLImageLibrary() : SDLImageDynLib("SDL3_image") {}
	};
}
