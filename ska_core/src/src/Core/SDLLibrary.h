#pragma once
#include "Base/DynamicLibrary/DynamicLibrary.h"
#include <SDL3/SDL.h>

namespace ska {

	template<int Id>
	struct SDLIdNamedFunction {
		static constexpr auto id = Id;
		static std::string name;
	};

	// The SDL3 functions used by the engine, loaded at run time (libSDL3)
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
		SDL_READ_SURFACE_PIXEL,
		SDL_GET_ERROR,
		SDL_FREE_SURFACE,
		SDL_FILL_RECT,
		SDL_DESTROY_TEXTURE,
		SDL_DELAY,
		SDL_CREATE_SURFACE,
		SDL_GET_PIXEL_FORMAT_FOR_MASKS,
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
		SDL_GET_TEXTURE_SIZE,
		SDL_SET_MAIN_READY,
		SDL_SHOW_CURSOR,
		SDL_HIDE_CURSOR,
		SDL_SET_RENDER_TARGET,
		SDL_CREATE_TEXTURE,
		SDL_IO_FROM_FILE,
		SDL_START_TEXT_INPUT,
		SDL_GET_KEYBOARD_FOCUS,
		SDL_SET_RENDER_DRAW_BLEND_MODE,
		SDL_GET_TEXTURE_COLOR_MOD,
		SDL_GET_TEXTURE_ALPHA_MOD,
		SDL_GET_TEXTURE_BLEND_MODE,
		SDL_SET_RENDER_VSYNC,
		SDL_GET_RENDERER_NAME,
		SDL_GET_GPU_RENDERER_DEVICE,
		SDL_GET_GPU_SHADER_FORMATS,
		SDL_CREATE_GPU_SHADER,
		SDL_RELEASE_GPU_SHADER,
		SDL_CREATE_GPU_RENDER_STATE,
		SDL_SET_GPU_RENDER_STATE,
		SDL_SET_GPU_RENDER_STATE_FRAGMENT_UNIFORMS,
		SDL_DESTROY_GPU_RENDER_STATE
	};

#define SKA_SDL_LIB_CALLS_DEFINE(ENUM, FUNCTION) \
			SKA_LIB_CALLS_DEFINE(SDLIdNamedFunction, static_cast<int>(SDLCalls::ENUM), FUNCTION)

	SKA_SDL_LIB_CALLS_DEFINE(SDL_UPPER_BLIT, bool(SDL_Surface*, const SDL_Rect*, SDL_Surface*, const SDL_Rect*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_INIT, bool(SDL_InitFlags));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_QUIT_F, void());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_POLL_EVENT, bool(SDL_Event*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_CLEAR, bool(SDL_Renderer*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_PRESENT, bool(SDL_Renderer*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_COLOR_MOD, bool(SDL_Texture*, Uint8, Uint8, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_BLEND_MOD, bool(SDL_Texture*, SDL_BlendMode));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_TEXTURE_ALPHA_MOD, bool(SDL_Texture*, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_SURFACE_ALPHA_MOD, bool(SDL_Surface*, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_COLOR_KEY, bool(SDL_Surface*, bool, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_MAP_RGBA, Uint32(SDL_Surface*, Uint8, Uint8, Uint8, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_TICKS, Uint64());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_READ_SURFACE_PIXEL, bool(SDL_Surface*, int, int, Uint8*, Uint8*, Uint8*, Uint8*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_ERROR, const char*());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_FREE_SURFACE, void(SDL_Surface*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_FILL_RECT, bool(SDL_Surface*, const SDL_Rect*, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DESTROY_TEXTURE, void(SDL_Texture*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DELAY, void(Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_SURFACE, SDL_Surface*(int, int, SDL_PixelFormat));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_PIXEL_FORMAT_FOR_MASKS, SDL_PixelFormat(int, Uint32, Uint32, Uint32, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_HINT, bool(const char*, const char*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_RENDERER, SDL_Renderer*(SDL_Window*, const char*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_TEXTURE_FROM_SURFACE, SDL_Texture*(SDL_Renderer*, SDL_Surface*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_RENDER_DRAW_COLOR, bool(SDL_Renderer*, Uint8, Uint8, Uint8, Uint8));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_DRAW_POINT, bool(SDL_Renderer*, float, float));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_DRAW_LINE, bool(SDL_Renderer*, float, float, float, float));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_FILL_RECT, bool(SDL_Renderer*, const SDL_FRect*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_COPY, bool(SDL_Renderer*, SDL_Texture*, const SDL_FRect*, const SDL_FRect*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RENDER_COPY_EX, bool(SDL_Renderer*, SDL_Texture*, const SDL_FRect*, const SDL_FRect*, double, const SDL_FPoint*, SDL_FlipMode));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DESTROY_RENDERER, void(SDL_Renderer*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_WINDOW, SDL_Window*(const char*, int, int, SDL_WindowFlags));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_WINDOW_ICON, bool(SDL_Window*, SDL_Surface*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SHOW_WINDOW, bool(SDL_Window*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DESTROY_WINDOW, void(SDL_Window*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SHOW_SIMPLE_MESSAGE_BOX, bool(SDL_MessageBoxFlags, const char*, const char*, SDL_Window*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_TEXTURE_SIZE, bool(SDL_Texture*, float*, float*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_MAIN_READY, void());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SHOW_CURSOR, bool());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_HIDE_CURSOR, bool());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_RENDER_TARGET, bool(SDL_Renderer*, SDL_Texture*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_TEXTURE, SDL_Texture*(SDL_Renderer*, SDL_PixelFormat, SDL_TextureAccess, int, int));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_IO_FROM_FILE, SDL_IOStream*(const char*, const char*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_START_TEXT_INPUT, bool(SDL_Window*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_KEYBOARD_FOCUS, SDL_Window*());
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_RENDER_DRAW_BLEND_MODE, bool(SDL_Renderer*, SDL_BlendMode));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_TEXTURE_COLOR_MOD, bool(SDL_Texture*, Uint8*, Uint8*, Uint8*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_TEXTURE_ALPHA_MOD, bool(SDL_Texture*, Uint8*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_TEXTURE_BLEND_MODE, bool(SDL_Texture*, SDL_BlendMode*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_RENDER_VSYNC, bool(SDL_Renderer*, int));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_RENDERER_NAME, const char*(SDL_Renderer*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_GPU_RENDERER_DEVICE, SDL_GPUDevice*(SDL_Renderer*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_GET_GPU_SHADER_FORMATS, SDL_GPUShaderFormat(SDL_GPUDevice*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_GPU_SHADER, SDL_GPUShader*(SDL_GPUDevice*, const SDL_GPUShaderCreateInfo*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_RELEASE_GPU_SHADER, void(SDL_GPUDevice*, SDL_GPUShader*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_CREATE_GPU_RENDER_STATE, SDL_GPURenderState*(SDL_Renderer*, const SDL_GPURenderStateCreateInfo*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_GPU_RENDER_STATE, bool(SDL_Renderer*, SDL_GPURenderState*));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_SET_GPU_RENDER_STATE_FRAGMENT_UNIFORMS, bool(SDL_GPURenderState*, Uint32, const void*, Uint32));
	SKA_SDL_LIB_CALLS_DEFINE(SDL_DESTROY_GPU_RENDER_STATE, void(SDL_GPURenderState*));

	#define SKA_SDL_DYN_LIB_NAME_ENTRY(ENUM) SDLIdNamedFunction<static_cast<int>(SDLCalls::ENUM)>

	using SDLDynLib = DynamicLibrary <
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_UPPER_BLIT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_INIT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_QUIT_F),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_POLL_EVENT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_CLEAR),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RENDER_PRESENT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_TEXTURE_COLOR_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_TEXTURE_BLEND_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_TEXTURE_ALPHA_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_SURFACE_ALPHA_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_COLOR_KEY),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_MAP_RGBA),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_TICKS),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_READ_SURFACE_PIXEL),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_ERROR),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_FREE_SURFACE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_FILL_RECT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_DESTROY_TEXTURE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_DELAY),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_SURFACE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_PIXEL_FORMAT_FOR_MASKS),
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
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_TEXTURE_SIZE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_MAIN_READY),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SHOW_CURSOR),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_HIDE_CURSOR),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_RENDER_TARGET),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_TEXTURE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_IO_FROM_FILE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_START_TEXT_INPUT),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_KEYBOARD_FOCUS),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_RENDER_DRAW_BLEND_MODE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_TEXTURE_COLOR_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_TEXTURE_ALPHA_MOD),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_TEXTURE_BLEND_MODE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_RENDER_VSYNC),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_RENDERER_NAME),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_GPU_RENDERER_DEVICE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_GET_GPU_SHADER_FORMATS),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_GPU_SHADER),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_RELEASE_GPU_SHADER),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_CREATE_GPU_RENDER_STATE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_GPU_RENDER_STATE),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_SET_GPU_RENDER_STATE_FRAGMENT_UNIFORMS),
		SKA_SDL_DYN_LIB_NAME_ENTRY(SDL_DESTROY_GPU_RENDER_STATE)
	>;

	// The SDL3 calls of the engine. The functions keep the conventions of the engine (written for SDL2) : those returning
	// an int return 0 on success, the drawing functions take integer rectangles.
	class SDLLibrary : public SDLDynLib {
		#define callSDL(enumIndex) call<SKA_SDL_DYN_LIB_NAME_ENTRY(enumIndex)>

		static int Result(bool success) { return success ? 0 : -1; }
		static SDL_FRect ToFloat(const SDL_Rect& rect) { return SDL_FRect{ static_cast<float>(rect.x), static_cast<float>(rect.y), static_cast<float>(rect.w), static_cast<float>(rect.h) }; }

	public:
		inline int init(Uint32 flags) const { return Result(callSDL(SDL_INIT)(flags)); }
		inline void quit() const { callSDL(SDL_QUIT_F)(); }
		inline bool setHint(const char* name, const char* value) const { return callSDL(SDL_SET_HINT)(name, value); }
		inline const char* getError() const { return callSDL(SDL_GET_ERROR)(); }
		inline Uint32 getTicks() const { return static_cast<Uint32>(callSDL(SDL_GET_TICKS)()); }
		inline void delay(Uint32 milliseconds) const { callSDL(SDL_DELAY)(milliseconds); }
		inline void setMainReady() const { callSDL(SDL_SET_MAIN_READY)(); }
		// 1 when an event was read
		inline int pollEvent(SDL_Event& event) const { return callSDL(SDL_POLL_EVENT)(&event) ? 1 : 0; }

		// Surfaces
		inline int upperBlit(SDL_Surface& src, const SDL_Rect* srcRect, SDL_Surface& dst, const SDL_Rect* dstRect) const { return Result(callSDL(SDL_UPPER_BLIT)(&src, srcRect, &dst, dstRect)); }
		inline int setSurfaceAlphaMod(SDL_Surface& surface, Uint8 alpha) const { return Result(callSDL(SDL_SET_SURFACE_ALPHA_MOD)(&surface, alpha)); }
		inline int setColorKey(SDL_Surface& surface, bool enabled, Uint32 key) const { return Result(callSDL(SDL_SET_COLOR_KEY)(&surface, enabled, key)); }
		inline Uint32 mapRGBA(SDL_Surface& surface, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const { return callSDL(SDL_MAP_RGBA)(&surface, r, g, b, a); }
		inline int readPixel(SDL_Surface& surface, int x, int y, Uint8* r, Uint8* g, Uint8* b, Uint8* a) const { return Result(callSDL(SDL_READ_SURFACE_PIXEL)(&surface, x, y, r, g, b, a)); }
		inline void freeSurface(SDL_Surface* surface) const { callSDL(SDL_FREE_SURFACE)(surface); }
		inline int fillRect(SDL_Surface& dst, const SDL_Rect* rect, Uint32 color) const { return Result(callSDL(SDL_FILL_RECT)(&dst, rect, color)); }
		// A surface of 32 bits per pixel with these masks (0 : the default RGBA format)
		inline SDL_Surface* createRGBSurface(int width, int height, Uint32 rmask, Uint32 gmask, Uint32 bmask, Uint32 amask) const {
			const auto format = rmask == 0 && gmask == 0 && bmask == 0 && amask == 0 ? SDL_PIXELFORMAT_RGBA32 : callSDL(SDL_GET_PIXEL_FORMAT_FOR_MASKS)(32, rmask, gmask, bmask, amask);
			return callSDL(SDL_CREATE_SURFACE)(width, height, format);
		}
		inline SDL_IOStream* rwFromFile(const std::string& file, const char* mode) const { return callSDL(SDL_IO_FROM_FILE)(file.c_str(), mode); }

		// Textures
		inline int setTextureAlphaMod(SDL_Texture& texture, Uint8 alpha) const { return Result(callSDL(SDL_SET_TEXTURE_ALPHA_MOD)(&texture, alpha)); }
		inline int setTextureColorMod(SDL_Texture& texture, Uint8 r, Uint8 g, Uint8 b) const { return Result(callSDL(SDL_SET_TEXTURE_COLOR_MOD)(&texture, r, g, b)); }
		inline int setTextureBlendMode(SDL_Texture& texture, SDL_BlendMode blendMode) const { return Result(callSDL(SDL_SET_TEXTURE_BLEND_MOD)(&texture, blendMode)); }
		inline int getTextureColorMod(SDL_Texture& texture, Uint8* r, Uint8* g, Uint8* b) const { return Result(callSDL(SDL_GET_TEXTURE_COLOR_MOD)(&texture, r, g, b)); }
		inline int getTextureAlphaMod(SDL_Texture& texture, Uint8* alpha) const { return Result(callSDL(SDL_GET_TEXTURE_ALPHA_MOD)(&texture, alpha)); }
		inline int getTextureBlendMode(SDL_Texture& texture, SDL_BlendMode* blendMode) const { return Result(callSDL(SDL_GET_TEXTURE_BLEND_MODE)(&texture, blendMode)); }
		inline void destroyTexture(SDL_Texture* texture) const { callSDL(SDL_DESTROY_TEXTURE)(texture); }
		inline int textureSize(SDL_Texture& texture, int* w, int* h) const {
			auto width = 0.F;
			auto height = 0.F;
			const auto success = callSDL(SDL_GET_TEXTURE_SIZE)(&texture, &width, &height);
			if (w != nullptr) *w = static_cast<int>(width);
			if (h != nullptr) *h = static_cast<int>(height);
			return Result(success);
		}

		// Renderer
		// "name" : a render driver ("gpu" : the shaders can be used), nullptr : the best one
		inline SDL_Renderer* createRenderer(SDL_Window& window, const char* name) const { return callSDL(SDL_CREATE_RENDERER)(&window, name); }
		inline const char* rendererName(SDL_Renderer& renderer) const { return callSDL(SDL_GET_RENDERER_NAME)(&renderer); }
		inline int setRenderVSync(SDL_Renderer& renderer, int vsync) const { return Result(callSDL(SDL_SET_RENDER_VSYNC)(&renderer, vsync)); }
		inline void destroyRenderer(SDL_Renderer* renderer) const { callSDL(SDL_DESTROY_RENDERER)(renderer); }
		inline SDL_Texture* createTextureFromSurface(SDL_Renderer& renderer, SDL_Surface& surface) const { return callSDL(SDL_CREATE_TEXTURE_FROM_SURFACE)(&renderer, &surface); }
		inline SDL_Texture* createTexture(SDL_Renderer& renderer, SDL_PixelFormat format, SDL_TextureAccess access, int w, int h) const { return callSDL(SDL_CREATE_TEXTURE)(&renderer, format, access, w, h); }
		inline int setRenderTarget(SDL_Renderer& renderer, SDL_Texture* texture) const { return Result(callSDL(SDL_SET_RENDER_TARGET)(&renderer, texture)); }
		inline int setRenderDrawBlendMode(SDL_Renderer& renderer, SDL_BlendMode blendMode) const { return Result(callSDL(SDL_SET_RENDER_DRAW_BLEND_MODE)(&renderer, blendMode)); }
		inline int setRenderDrawColor(SDL_Renderer& renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const { return Result(callSDL(SDL_SET_RENDER_DRAW_COLOR)(&renderer, r, g, b, a)); }
		inline int renderClear(SDL_Renderer& renderer) const { return Result(callSDL(SDL_RENDER_CLEAR)(&renderer)); }
		inline void renderPresent(SDL_Renderer& renderer) const { callSDL(SDL_RENDER_PRESENT)(&renderer); }
		inline int renderDrawPoint(SDL_Renderer& renderer, int x, int y) const { return Result(callSDL(SDL_RENDER_DRAW_POINT)(&renderer, static_cast<float>(x), static_cast<float>(y))); }
		inline int renderDrawLine(SDL_Renderer& renderer, int x1, int y1, int x2, int y2) const {
			return Result(callSDL(SDL_RENDER_DRAW_LINE)(&renderer, static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2)));
		}
		inline int renderFillRect(SDL_Renderer& renderer, const SDL_Rect* rect) const {
			const auto floatRect = rect == nullptr ? SDL_FRect{} : ToFloat(*rect);
			return Result(callSDL(SDL_RENDER_FILL_RECT)(&renderer, rect == nullptr ? nullptr : &floatRect));
		}
		inline int renderCopy(SDL_Renderer& renderer, SDL_Texture& texture, const SDL_Rect* src, const SDL_Rect* dst) const {
			const auto floatSrc = src == nullptr ? SDL_FRect{} : ToFloat(*src);
			const auto floatDst = dst == nullptr ? SDL_FRect{} : ToFloat(*dst);
			return Result(callSDL(SDL_RENDER_COPY)(&renderer, &texture, src == nullptr ? nullptr : &floatSrc, dst == nullptr ? nullptr : &floatDst));
		}
		inline int renderCopyEx(SDL_Renderer& renderer, SDL_Texture& texture, const SDL_Rect* src, const SDL_Rect* dst, double angle, const SDL_Point* center, SDL_FlipMode flip) const {
			const auto floatSrc = src == nullptr ? SDL_FRect{} : ToFloat(*src);
			const auto floatDst = dst == nullptr ? SDL_FRect{} : ToFloat(*dst);
			const auto floatCenter = center == nullptr ? SDL_FPoint{} : SDL_FPoint{ static_cast<float>(center->x), static_cast<float>(center->y) };
			return Result(callSDL(SDL_RENDER_COPY_EX)(&renderer, &texture, src == nullptr ? nullptr : &floatSrc, dst == nullptr ? nullptr : &floatDst, angle, center == nullptr ? nullptr : &floatCenter, flip));
		}

		// Shaders (the "gpu" render driver)
		inline SDL_GPUDevice* gpuRendererDevice(SDL_Renderer& renderer) const { return callSDL(SDL_GET_GPU_RENDERER_DEVICE)(&renderer); }
		inline SDL_GPUShaderFormat gpuShaderFormats(SDL_GPUDevice& device) const { return callSDL(SDL_GET_GPU_SHADER_FORMATS)(&device); }
		inline SDL_GPUShader* createGPUShader(SDL_GPUDevice& device, const SDL_GPUShaderCreateInfo& info) const { return callSDL(SDL_CREATE_GPU_SHADER)(&device, &info); }
		inline void releaseGPUShader(SDL_GPUDevice& device, SDL_GPUShader* shader) const { callSDL(SDL_RELEASE_GPU_SHADER)(&device, shader); }
		inline SDL_GPURenderState* createGPURenderState(SDL_Renderer& renderer, const SDL_GPURenderStateCreateInfo& info) const { return callSDL(SDL_CREATE_GPU_RENDER_STATE)(&renderer, &info); }
		inline int setGPURenderState(SDL_Renderer& renderer, SDL_GPURenderState* state) const { return Result(callSDL(SDL_SET_GPU_RENDER_STATE)(&renderer, state)); }
		inline int setGPURenderStateFragmentUniforms(SDL_GPURenderState& state, Uint32 slot, const void* data, Uint32 length) const {
			return Result(callSDL(SDL_SET_GPU_RENDER_STATE_FRAGMENT_UNIFORMS)(&state, slot, data, length));
		}
		inline void destroyGPURenderState(SDL_GPURenderState* state) const { callSDL(SDL_DESTROY_GPU_RENDER_STATE)(state); }

		// Window
		inline SDL_Window* createWindow(const std::string& title, int w, int h, SDL_WindowFlags flags) const { return callSDL(SDL_CREATE_WINDOW)(title.c_str(), w, h, flags); }
		inline void setWindowIcon(SDL_Window& window, SDL_Surface* icon) const { callSDL(SDL_SET_WINDOW_ICON)(&window, icon); }
		inline void showWindow(SDL_Window& window) const { callSDL(SDL_SHOW_WINDOW)(&window); }
		inline void destroyWindow(SDL_Window* window) const { callSDL(SDL_DESTROY_WINDOW)(window); }
		inline void showSimpleMessageBox(SDL_MessageBoxFlags flags, const std::string& title, const std::string& message, SDL_Window* window) const {
			callSDL(SDL_SHOW_SIMPLE_MESSAGE_BOX)(flags, title.c_str(), message.c_str(), window);
		}
		inline void showCursor(bool show) const {
			if (show) callSDL(SDL_SHOW_CURSOR)(); else callSDL(SDL_HIDE_CURSOR)();
		}
		// The text typed in the window with the keyboard focus
		inline void startTextInput() const {
			auto* window = callSDL(SDL_GET_KEYBOARD_FOCUS)();
			if (window != nullptr) {
				callSDL(SDL_START_TEXT_INPUT)(window);
			}
		}

		static SDLLibrary& get();

		// SDL quits when the program ends, after every module (the sound module closes its device after the graphics)
		~SDLLibrary() { quit(); }

	private:
		SDLLibrary() : SDLDynLib("SDL3") {}

		#undef callSDL
	};
}
