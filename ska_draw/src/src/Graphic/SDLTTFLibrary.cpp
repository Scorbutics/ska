#include "Graphic/__internalConfig/LoggerConfig.h"
#include "SDLTTFLibrary.h"

#define SKA_TTF_LIB_NAME_DEFINE(ENUM, NAME) \
	SKA_LIB_NAME_DEFINE(ska::TTFIdNamedFunction, static_cast<int>(ska::TTFCalls::ENUM), NAME)

SKA_TTF_LIB_NAME_DEFINE(TTF_INIT, "TTF_Init");
SKA_TTF_LIB_NAME_DEFINE(TTF_QUIT, "TTF_Quit");
SKA_TTF_LIB_NAME_DEFINE(TTF_OPEN_FONT, "TTF_OpenFont");
SKA_TTF_LIB_NAME_DEFINE(TTF_CLOSE_FONT, "TTF_CloseFont");
SKA_TTF_LIB_NAME_DEFINE(TTF_RENDER_TEXT_BLENDED, "TTF_RenderText_Blended");

const ska::SDLTTFLibrary& ska::SDLTTFLibrary::get() {
	static const SDLTTFLibrary instance;
	return instance;
}
