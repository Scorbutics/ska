#pragma once
#include "Base/DynamicLibrary/DynamicLibrary.h"
#include <SDL.h>

namespace ska {

	template<int Id>
	struct SDLIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	enum class SDLCalls {
		SDL_UPPER_BLIT,
		SDL_INIT,
		SDL_QUIT_F,
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
		SDL_CREATE_RGB_SURFACE,
		SDL_SET_HINT,
		SDL_CREATE_RENDERER, 
		SDL_CREATE_TEXTURE_FROM_SURFACE,
		SDL_SET_RENDER_DRAW_COLOR, 
		SDL_RENDER_DRAW_POINT,
		SDL_RENDER_DRAW_LINE,
		SDL_RENDER_FILL_RECT,
		SDL_RENDER_COPY, 
		SDL_RENDER_COPY_EX,
		SDL_DESTROY_RENDERER,
		SDL_CREATE_WINDOW, 
		SDL_SET_WINDOW_ICON, 
		SDL_SHOW_WINDOW, 
		SDL_DESTROY_WINDOW, 
		SDL_SHOW_SIMPLE_MESSAGE_BOX,
		SDL_QUERY_TEXTURE,
		SDL_SET_MAIN_READY,
		SDL_SHOW_CURSOR,
		SDL_SET_RENDER_TARGET,
		SDL_CREATE_TEXTURE,
		SDL_SET_RENDER_DRAW_BLEND_MODE
	};

#define SKA_SDL_LIB_CALLS_DEFINE(ENUM, FUNCTION) \
			SKA_LIB_CALLS_DEFINE(SDLIdNamedFunction, static_cast<int>(SDLCalls::ENUM), FUNCTION)

	SKA_SDL_LIB_CALLS_DEFINE(SDL_UPPER_BLIT, int(SDL_Surface*, const SDL_Rect*, SDL_Surface*, SDL_Rect*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_POLL_EVENT, int(SDL_Event*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_INIT, int(Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_QUIT_F, void());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_HINT, int(const char*, const char*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_PRESENT, void(SDL_Renderer*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_CLEAR, int(SDL_Renderer*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_ALPHA_MOD, int(SDL_Texture*, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_SURFACE_ALPHA_MOD, int(SDL_Surface*, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_COLOR_MOD, int(SDL_Texture*, Uint8, Uint8, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_BLEND_MOD, int(SDL_Texture*, SDL_BlendMode));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_COLOR_KEY, int(SDL_Surface*, int, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_MAP_RGBA, Uint32(const SDL_PixelFormat*, Uint8, Uint8, Uint8, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_TICKS, Uint32());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_RGB, void(Uint32, const SDL_PixelFormat*, Uint8*, Uint8*, Uint8*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_ERROR, const char*());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_FREE_SURFACE, void(SDL_Surface*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_FILL_RECT, int(SDL_Surface *, const SDL_Rect *, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DESTROY_TEXTURE, void(SDL_Texture *));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DELAY, void(Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_RGB_SURFACE, SDL_Surface* (Uint32, int, int, int, Uint32, Uint32, Uint32, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_RENDERER, SDL_Renderer*(SDL_Window *, int, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_TEXTURE_FROM_SURFACE, SDL_Texture*(SDL_Renderer*, SDL_Surface*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_RENDER_DRAW_COLOR, int(SDL_Renderer*, Uint8, Uint8, Uint8, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_DRAW_POINT, int(SDL_Renderer *, int, int));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_DRAW_LINE, int(SDL_Renderer*, int, int, int, int));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_FILL_RECT, int(SDL_Renderer*, const SDL_Rect*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_COPY, int(SDL_Renderer*, SDL_Texture*, const SDL_Rect*, const SDL_Rect*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_COPY_EX, int(SDL_Renderer*, SDL_Texture*, const SDL_Rect*, const SDL_Rect*, const double, const SDL_Point*, const SDL_RendererFlip));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DESTROY_RENDERER, void(SDL_Renderer*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_WINDOW, SDL_Window*(const char*, int, int, int, int, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_WINDOW_ICON, void(SDL_Window*, SDL_Surface*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SHOW_WINDOW, void(SDL_Window *));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DESTROY_WINDOW, void(SDL_Window*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SHOW_SIMPLE_MESSAGE_BOX, int(Uint32, const char*, const char*, SDL_Window*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_QUERY_TEXTURE, int(SDL_Texture*, Uint32*, int*, int*, int*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_MAIN_READY, void());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SHOW_CURSOR, int(int toggle));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_RENDER_TARGET, int(SDL_Renderer* renderer, SDL_Texture*  texture));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_TEXTURE, SDL_Texture*(SDL_Renderer* renderer, Uint32 format, int access, int w, int h) );
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_RENDER_DRAW_BLEND_MODE, int(SDL_Renderer* renderer, SDL_BlendMode blendMode));

	#define SKA_SDL_DYN_LIB_NAME_ENTRY(ENUM) SDLIdNamedFunction<static_cast<int>(SDLCalls::ENUM)>

	using SDLDynLib = DynamicLibrary <
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_UPPER_BLIT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_POLL_EVENT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_INIT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_QUIT_F),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_PRESENT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_CLEAR),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_TEXTURE_ALPHA_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_SURFACE_ALPHA_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_TEXTURE_COLOR_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_TEXTURE_BLEND_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_COLOR_KEY),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_MAP_RGBA),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_TICKS),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_RGB),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_ERROR),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_FREE_SURFACE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_FILL_RECT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_DESTROY_TEXTURE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_DELAY),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_RGB_SURFACE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_HINT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_RENDERER),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_TEXTURE_FROM_SURFACE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_RENDER_DRAW_COLOR),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_DRAW_POINT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_DRAW_LINE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_FILL_RECT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_COPY),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_COPY_EX),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_DESTROY_RENDERER),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_WINDOW),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_WINDOW_ICON),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SHOW_WINDOW),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_DESTROY_WINDOW),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SHOW_SIMPLE_MESSAGE_BOX),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_QUERY_TEXTURE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_MAIN_READY),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SHOW_CURSOR),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_RENDER_TARGET),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_TEXTURE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_RENDER_DRAW_BLEND_MODE)
	>;


	class SDLLibrary : public SDLDynLib {
        #define callSDL(enumIndex) call<SKA_SDL_DYN_LIB_NAME_ENTRY(enumIndex)>

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

		void quit() const {
			callSDL(SDL_QUIT_F)();
		}

		int setHint(const char* str, const char* text) const {
			return callSDL(SDL_SET_HINT)(std::move(str), std::move(text));
		}

		SDL_Renderer* createRenderer(SDL_Window& window, int index, Uint32 flags) const {
			return callSDL(SDL_CREATE_RENDERER)(&window, std::move(index), std::move(flags));
		}

		SDL_Texture* createTextureFromSurface(SDL_Renderer& renderer, SDL_Surface& surface) const {
			return callSDL(SDL_CREATE_TEXTURE_FROM_SURFACE)(&renderer, &surface);
		}
		
		int setRenderDrawColor(SDL_Renderer& renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {			
			return callSDL(SDL_SET_RENDER_DRAW_COLOR)(&renderer, std::move(r), std::move(g), std::move(b), std::move(a));
		}
		int renderFillRect(SDL_Renderer& renderer, const SDL_Rect* rect) const {
			return callSDL(SDL_RENDER_FILL_RECT)(&renderer, std::move(rect));
		}

		int renderCopy(SDL_Renderer& renderer, SDL_Texture& texture, const SDL_Rect* src, const SDL_Rect* dst) const {
			return callSDL(SDL_RENDER_COPY)(&renderer, &texture, src, dst);
		}

		int renderCopyEx(SDL_Renderer& renderer, SDL_Texture& texture, const SDL_Rect* src, const SDL_Rect* dst, double angle, const SDL_Point* center, const SDL_RendererFlip flip) const {
			return callSDL(SDL_RENDER_COPY_EX)(&renderer, &texture, src, dst, angle, center, flip);
		}

		void destroyRenderer(SDL_Renderer* renderer) const {
			return callSDL(SDL_DESTROY_RENDERER)(std::move(renderer));
		}

		SDL_Window* createWindow(const std::string& title, int x, int y, int w, int h, Uint32 flags) const {
			return callSDL(SDL_CREATE_WINDOW)(title.c_str(), std::move(x), std::move(y), std::move(w), std::move(h), std::move(flags));
		}

		void setWindowIcon(SDL_Window& window, SDL_Surface* icon) const {
			return callSDL(SDL_SET_WINDOW_ICON)(&window, std::move(icon));
		}

		void showWindow(SDL_Window& window) const {
			return callSDL(SDL_SHOW_WINDOW)(&window);
		}

		void destroyWindow(SDL_Window* window) const {
			return callSDL(SDL_DESTROY_WINDOW)(window);
		}

		void showSimpleMessageBox(Uint32 flags, const std::string& title, const std::string& message, SDL_Window* window) const {
			callSDL(SDL_SHOW_SIMPLE_MESSAGE_BOX)(std::move(flags), title.c_str(), message.c_str(), window);
		}

		int queryTexture(SDL_Texture& actTexture, Uint32* format, int* access, int* w, int* h) const {
			return callSDL(SDL_QUERY_TEXTURE)(&actTexture, format, access, w, h);
		}

		int renderDrawPoint(SDL_Renderer& renderer, int x, int y) const {
			return callSDL(SDL_RENDER_DRAW_POINT)(&renderer, x, y);
		}

		int renderDrawLine(SDL_Renderer& renderer, int x1, int y1, int x2, int y2) const {
			return callSDL(SDL_RENDER_DRAW_LINE)(&renderer, x1, y1, x2, y2);
		}

		void setMainReady() const {
			callSDL(SDL_SET_MAIN_READY)();
		}

		int showCursor(bool b) const {
			return callSDL(SDL_SHOW_CURSOR)(static_cast<int>(b));
		}

		SDL_Texture* createTexture(SDL_Renderer& renderer, Uint32 format, int access, int w, int h)  const {
			return callSDL(SDL_CREATE_TEXTURE)(&renderer, format, access, w, h);
		}

		int setRenderTarget(SDL_Renderer& renderer, SDL_Texture* texture) const {
			return callSDL(SDL_SET_RENDER_TARGET)(&renderer, texture);
		}

		int setRenderDrawBlendMode(SDL_Renderer& renderer, SDL_BlendMode blendMode) const {
			return callSDL(SDL_SET_RENDER_DRAW_BLEND_MODE)(&renderer, blendMode);
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
