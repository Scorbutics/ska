#include "Graphic/__internalConfig/LoggerConfig.h"
#include "SDLGifLibrary.h"

#define SKA_GIF_LIB_NAME_DEFINE(ENUM, NAME) \
	SKA_LIB_NAME_DEFINE(ska::SDLGifIdNamedFunction, static_cast<int>(ska::SDLGifCalls::ENUM), NAME)

SKA_GIF_LIB_NAME_DEFINE(GIF_TEXTURE, "CEV_gifTexture");
SKA_GIF_LIB_NAME_DEFINE(GIF_FRAME_NEXT, "CEV_gifFrameNext");
SKA_GIF_LIB_NAME_DEFINE(GIF_TIME_SET, "CEV_gifTimeSet");
SKA_GIF_LIB_NAME_DEFINE(GIF_LOOP_MODE, "CEV_gifLoopMode");
SKA_GIF_LIB_NAME_DEFINE(GIF_LOOP_RESET, "CEV_gifLoopReset");
SKA_GIF_LIB_NAME_DEFINE(GIF_ANIM_AUTO, "CEV_gifAnimAuto");
SKA_GIF_LIB_NAME_DEFINE(GIF_ANIM_FREE, "CEV_gifAnimFree");

const ska::SDLGifLibrary& ska::SDLGifLibrary::get() {
	static SDLGifLibrary instance;
	return instance;
}
