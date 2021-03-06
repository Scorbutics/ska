#include "Core/__internalConfig/LoggerConfig.h"
#include "SDLLibrary.h"

#define SKA_SDL_LIB_NAME_DEFINE(ENUM, NAME) \
	SKA_LIB_NAME_DEFINE(ska::SDLIdNamedFunction, static_cast<int>(ska::SDLCalls::ENUM), NAME)

SKA_SDL_LIB_NAME_DEFINE(SDL_UPPER_BLIT, "SDL_UpperBlit");
SKA_SDL_LIB_NAME_DEFINE(SDL_POLL_EVENT, "SDL_PollEvent");
SKA_SDL_LIB_NAME_DEFINE(SDL_INIT, "SDL_Init");
SKA_SDL_LIB_NAME_DEFINE(SDL_QUIT_F, "SDL_Quit");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_HINT, "SDL_SetHint");
SKA_SDL_LIB_NAME_DEFINE(SDL_RENDER_PRESENT, "SDL_RenderPresent");
SKA_SDL_LIB_NAME_DEFINE(SDL_RENDER_CLEAR, "SDL_RenderClear");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_TEXTURE_ALPHA_MOD, "SDL_SetTextureAlphaMod");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_SURFACE_ALPHA_MOD, "SDL_SetSurfaceAlphaMod");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_TEXTURE_COLOR_MOD, "SDL_SetTextureBlendMode");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_TEXTURE_BLEND_MOD, "SDL_SetTextureBlendMode");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_COLOR_KEY, "SDL_SetColorKey");
SKA_SDL_LIB_NAME_DEFINE(SDL_MAP_RGBA, "SDL_MapRGBA");
SKA_SDL_LIB_NAME_DEFINE(SDL_GET_TICKS, "SDL_GetTicks");
SKA_SDL_LIB_NAME_DEFINE(SDL_GET_RGB, "SDL_GetRGB");
SKA_SDL_LIB_NAME_DEFINE(SDL_GET_ERROR, "SDL_GetError");
SKA_SDL_LIB_NAME_DEFINE(SDL_FREE_SURFACE, "SDL_FreeSurface");
SKA_SDL_LIB_NAME_DEFINE(SDL_FILL_RECT, "SDL_FillRect");
SKA_SDL_LIB_NAME_DEFINE(SDL_DESTROY_TEXTURE, "SDL_DestroyTexture");
SKA_SDL_LIB_NAME_DEFINE(SDL_DELAY, "SDL_Delay");
SKA_SDL_LIB_NAME_DEFINE(SDL_CREATE_RGB_SURFACE, "SDL_CreateRGBSurface");
SKA_SDL_LIB_NAME_DEFINE(SDL_CREATE_RENDERER, "SDL_CreateRenderer");
SKA_SDL_LIB_NAME_DEFINE(SDL_CREATE_TEXTURE, "SDL_CreateTexture");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_RENDER_TARGET, "SDL_SetRenderTarget");
SKA_SDL_LIB_NAME_DEFINE(SDL_CREATE_TEXTURE_FROM_SURFACE, "SDL_CreateTextureFromSurface");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_RENDER_DRAW_COLOR, "SDL_SetRenderDrawColor");
SKA_SDL_LIB_NAME_DEFINE(SDL_RENDER_DRAW_POINT, "SDL_RenderDrawPoint");
SKA_SDL_LIB_NAME_DEFINE(SDL_RENDER_DRAW_LINE, "SDL_RenderDrawLine");
SKA_SDL_LIB_NAME_DEFINE(SDL_RENDER_FILL_RECT, "SDL_RenderFillRect");
SKA_SDL_LIB_NAME_DEFINE(SDL_RENDER_COPY, "SDL_RenderCopy");
SKA_SDL_LIB_NAME_DEFINE(SDL_RENDER_COPY_EX, "SDL_RenderCopyEx");
SKA_SDL_LIB_NAME_DEFINE(SDL_DESTROY_RENDERER, "SDL_DestroyRenderer");
SKA_SDL_LIB_NAME_DEFINE(SDL_CREATE_WINDOW, "SDL_CreateWindow");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_WINDOW_ICON, "SDL_SetWindowIcon");
SKA_SDL_LIB_NAME_DEFINE(SDL_SHOW_WINDOW, "SDL_ShowWindow");
SKA_SDL_LIB_NAME_DEFINE(SDL_DESTROY_WINDOW, "SDL_DestroyWindow");
SKA_SDL_LIB_NAME_DEFINE(SDL_SHOW_SIMPLE_MESSAGE_BOX, "SDL_ShowSimpleMessageBox");
SKA_SDL_LIB_NAME_DEFINE(SDL_QUERY_TEXTURE, "SDL_QueryTexture");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_MAIN_READY, "SDL_SetMainReady");
SKA_SDL_LIB_NAME_DEFINE(SDL_SHOW_CURSOR, "SDL_ShowCursor");
SKA_SDL_LIB_NAME_DEFINE(SDL_RW_FROM_FILE, "SDL_RWFromFile");
SKA_SDL_LIB_NAME_DEFINE(SDL_START_TEXT_INPUT, "SDL_StartTextInput");
SKA_SDL_LIB_NAME_DEFINE(SDL_SET_RENDER_DRAW_BLEND_MODE, "SDL_SetRenderDrawBlendMode");

ska::SDLLibrary& ska::SDLLibrary::get() {
	static SDLLibrary instance;
	return instance;
}
