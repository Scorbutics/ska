#pragma once
#include "Utils/DynamicLibrary.h"
#include <SDL.h>

#define SKA_SDL_LIB_CALLS_DEFINE(ENUM, FUNCTION, NAME) \
			SKA_LIB_CALLS_DEFINE(SDLIdNamedFunction, ENUM, FUNCTION, NAME)

namespace ska {

	template<int Id>
	struct SDLIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

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

	SKA_SDL_LIB_CALLS_DEFINE(SDL_UPPER_BLIT, int(SDL_Surface*, const SDL_Rect*, SDL_Surface*, SDL_Rect*), "SDL_UpperBlit");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_POLL_EVENT, int(SDL_Event*), "SDL_PollEvent");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_INIT, int(Uint32), "SDL_Init");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_PRESENT, void(SDL_Renderer*), "SDL_RenderPresent");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_CLEAR, int(SDL_Renderer*), "SDL_RenderClear");
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
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DELAY, void(Uint32), "SDL_Delay");
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_RGB_SURFACE, SDL_Surface* (Uint32, int, int, int, Uint32, Uint32, Uint32, Uint32), "SDL_CreateRGBSurface");

	using SDLDynLib = DynamicLibrary <
		SDLIdNamedFunction<SDL_UPPER_BLIT>,
		SDLIdNamedFunction<SDL_POLL_EVENT>,
		SDLIdNamedFunction<SDL_INIT>,
		SDLIdNamedFunction<SDL_RENDER_PRESENT>,
		SDLIdNamedFunction<SDL_RENDER_CLEAR>,
		SDLIdNamedFunction<SDL_SET_TEXTURE_ALPHA_MOD>,
		SDLIdNamedFunction<SDL_SET_SURFACE_ALPHA_MOD>,
		SDLIdNamedFunction<SDL_SET_TEXTURE_COLOR_MOD>,
		SDLIdNamedFunction<SDL_SET_TEXTURE_BLEND_MOD>,
		SDLIdNamedFunction<SDL_SET_COLOR_KEY>,
		SDLIdNamedFunction<SDL_MAP_RGBA>,
		SDLIdNamedFunction<SDL_GET_TICKS>,
		SDLIdNamedFunction<SDL_GET_RGB>,
		SDLIdNamedFunction<SDL_GET_ERROR>,
		SDLIdNamedFunction<SDL_FREE_SURFACE>,
		SDLIdNamedFunction<SDL_FILL_RECT>,
		SDLIdNamedFunction<SDL_DESTROY_TEXTURE>,
		SDLIdNamedFunction<SDL_DELAY>,
		SDLIdNamedFunction<SDL_CREATE_RGB_SURFACE>>;


	class SDLLibrary : public SDLDynLib {
        #define callSDL(enumIndex) call<SDLIdNamedFunction<enumIndex>>

	public:
		int init(Uint32 flags) const {
			return callSDL(SDL_INIT)(std::move(flags));
		}

        int upperBlit(SDL_Surface& src, const SDL_Rect* srcRect, SDL_Surface& dst, SDL_Rect* dstRect) const {
			return callSDL(SDL_UPPER_BLIT)(&src, std::move(srcRect), &dst, std::move(dstRect));
		}

        int pollEvent(SDL_Event& event) const {
            return callSDL(SDL_POLL_EVENT)(&event);
        }

        void renderPresent(SDL_Renderer& renderer) const {
            callSDL(SDL_RENDER_PRESENT)(&renderer);
        }

        int renderClear(SDL_Renderer& renderer) const {
            return callSDL(SDL_RENDER_CLEAR)(&renderer);
        }

        int setTextureAlphaMod(SDL_Texture& texture, Uint8 alpha) const {
            return callSDL(SDL_SET_TEXTURE_ALPHA_MOD)(&texture, std::move(alpha));
        }

        int setSurfaceAlphaMod(SDL_Surface& surface, Uint8 alpha) const {
            return callSDL(SDL_SET_SURFACE_ALPHA_MOD)(&surface, std::move(alpha));
        }

        int setTextureColorMod(SDL_Texture& texture, Uint8 r, Uint8 g, Uint8 b) const {
            return callSDL(SDL_SET_TEXTURE_COLOR_MOD)(&texture, std::move(r), std::move(g), std::move(b));
        }

        int setTextureBlendMode(SDL_Texture& texture, SDL_BlendMode blendMode) const {
            return callSDL(SDL_SET_TEXTURE_BLEND_MOD)(&texture, std::move(blendMode));
        }

        int setColorKey(SDL_Surface& surface, int flag, Uint32 key) const {
            return callSDL(SDL_SET_COLOR_KEY)(&surface, std::move(flag), std::move(key));
        }

        Uint32 mapRGBA(const SDL_PixelFormat& format, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
            return callSDL(SDL_MAP_RGBA)(&format, std::move(r), std::move(g), std::move(b), std::move(a));
        }

        Uint32 getTicks() const {
            return callSDL(SDL_GET_TICKS)();
        }

        void getRGB(Uint32 pixel, const SDL_PixelFormat& format, Uint8* r, Uint8* g, Uint8* b) const {
            callSDL(SDL_GET_RGB)(std::move(pixel), &format, std::move(r), std::move(g), std::move(b));
        }

        const char* getError() const {
            return callSDL(SDL_GET_ERROR)();
        }

        void freeSurface(SDL_Surface* surface) const {
            callSDL(SDL_FREE_SURFACE)(std::move(surface));
        }

        int fillRect(SDL_Surface& dst, const SDL_Rect* rect, Uint32 color) const {
            return callSDL(SDL_FILL_RECT)(&dst, std::move(rect), std::move(color));
        }

        void destroyTexture(SDL_Texture* texture) const {
            callSDL(SDL_DESTROY_TEXTURE)(std::move(texture));
        }

        void delay(Uint32 delay) const {
            callSDL(SDL_DELAY)(std::move(delay));
        }

        SDL_Surface* createRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) const {
            return callSDL(SDL_CREATE_RGB_SURFACE)(
                std::move(flags), std::move(width), std::move(height),
                std::move(depth), std::move(Rmask), std::move(Gmask),
                std::move(Bmask), std::move(Amask));
        }

		static SDLLibrary& get() {
			static SDLLibrary instance;
			return instance;
		}

	private:
		SDLLibrary() : SDLDynLib("SDL2") {}

		#undef callSDL
	};
}
