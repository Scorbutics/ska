#pragma once
#include "Utils/DynamicLibrary.h"
#include <SDL.h>
#include <gsl/pointers>

#define SKA_IMG_LIB_CALLS_DEFINE(ENUM, FUNCTION, NAME) \
			SKA_LIB_CALLS_DEFINE(SDLImageIdNamedFunction, ENUM, FUNCTION, NAME)

namespace ska {

	template<int Id>
	struct SDLImageIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	enum SDLImageCalls {
		IMG_INIT,
		IMG_QUIT,
		IMG_LOAD
	};
	
	SKA_IMG_LIB_CALLS_DEFINE(IMG_INIT, int(int), "IMG_Init");
	SKA_IMG_LIB_CALLS_DEFINE(IMG_LOAD, SDL_Surface* (const char *), "IMG_Load");
	SKA_IMG_LIB_CALLS_DEFINE(IMG_QUIT, void(), "IMG_Quit");

	using SDLImageDynLib = DynamicLibrary <
		SDLImageIdNamedFunction<IMG_INIT>,
		SDLImageIdNamedFunction<IMG_QUIT>,
		SDLImageIdNamedFunction<IMG_LOAD>>;
	

	class SDLImageLibrary : public SDLImageDynLib {
		#define callSDLImage(enumIndex) call<SDLImageIdNamedFunction<enumIndex>>
	public:
		int init(int flags) const {
			return callSDLImage(IMG_INIT)(std::move(flags));
		}

		SDL_Surface* load(const std::string& file) const {
			return callSDLImage(IMG_LOAD)(file.c_str());
		}

		void quit() const {
			callSDLImage(IMG_QUIT)();
		}

		static const SDLImageLibrary& get() {
			static SDLImageLibrary instance;
			return instance;
		}
		#undef callSDLImage
	private:
		SDLImageLibrary() : SDLImageDynLib("SDL2_image") {}
	};
}
