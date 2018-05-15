#pragma once
#include "Utils/DynamicLibrary.h"
#include <SDL.h>

#define SKA_SDL_LIB_CALLS_DEFINE(ENUM, FUNCTION, NAME) \
			SKA_LIB_CALLS_DEFINE(SDLIdNamedFunction, ENUM, FUNCTION, NAME)

namespace ska {

	enum SDLCalls {
		SDL_UPPER_BLIT,
		SDL_INIT,
		SDL_POLL_EVENT,
		SDL_RENDER_CLEAR,
		SDL_RENDER_PRESENT,
		SDL_SET_TEXTURE_COLOR_MOD,
		SDL_SET_TEXTURE_BLEND_MOD,
		SDL_SET_TEXTURE_ALPHA_MOD,
		SDL_SET_SURFACE_ALPHA_MOD,
		SDL_SET_COLOR_KEY,
		SDL_MAP_RGBA,
		SDL_GET_TICKS,
		SDL_GET_RGB,
		SDL_GET_ERROR,
		SDL_FREE_SURFACE,
		SDL_FILL_RECT,
		SDL_DESTROY_TEXTURE,
		SDL_DELAY,
		SDL_CREATE_RGB_SURFACE
	};

	template<int Id>
	struct SDLIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};
		
	SKA_SDL_LIB_CALLS_DEFINE(SDL_UPPER_BLIT, void(), "SDL_UpperBlit");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_POLL_EVENT, void(), "SDL_PollEvent");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_INIT, int(Uint32), "SDL_Init");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_PRESENT, void(), "SDL_RenderPresent");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_CLEAR, void(), "SDL_RenderClear");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_ALPHA_MOD, int (SDL_Texture*, Uint8), "SDL_SetTextureAlphaMod");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_SURFACE_ALPHA_MOD, int (SDL_Surface*, Uint8), "SDL_SetSurfaceAlphaMod");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_COLOR_MOD, int (SDL_Texture*, Uint8, Uint8, Uint8), "SDL_SetTextureBlendMode");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_BLEND_MOD, int(SDL_Texture*, SDL_BlendMode), "SDL_SetTextureBlendMode");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_COLOR_KEY, int(SDL_Surface*, int, Uint32), "SDL_SetColorKey");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_MAP_RGBA, Uint32(const SDL_PixelFormat*, Uint8, Uint8, Uint8, Uint8), "SDL_MapRGBA");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_TICKS, Uint32(), "SDL_GetTicks");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_RGB, void (Uint32, const SDL_PixelFormat*, Uint8*, Uint8*, Uint8*), "SDL_GetRGB");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_ERROR, const char*(), "SDL_GetError");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_FREE_SURFACE, void(SDL_Surface*), "SDL_FreeSurface");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_FILL_RECT, int (SDL_Surface *, const SDL_Rect *, Uint32), "SDL_FillRect");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DESTROY_TEXTURE, void(SDL_Texture *), "SDL_DestroyTexture");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DELAY, Uint32(), "SDL_Delay");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_RGB_SURFACE, SDL_Surface *(Uint32, int, int, int, Uint32, Uint32, Uint32, Uint32), "SDL_CreateRGBSurface");
	
	using SDLDynLib = DynamicLibrary <
		SDLIdNamedFunction<SDL_UPPER_BLIT>,
		SDLIdNamedFunction<SDL_INIT>,
		SDLIdNamedFunction<SDL_POLL_EVENT>,
		SDLIdNamedFunction<SDL_RENDER_PRESENT>,
		SDLIdNamedFunction<SDL_RENDER_CLEAR>>;
	

	class SDLLibrary : public SDLDynLib {
	public:
		int init(Uint32 flags) const {
			return call<SDLIdNamedFunction<SDL_INIT>>(std::move(flags));
		}

		static SDLLibrary& get() {
			static SDLLibrary instance;
			return instance;
		}

	private:
		SDLLibrary() : SDLDynLib("SDL") {}
	};
}
