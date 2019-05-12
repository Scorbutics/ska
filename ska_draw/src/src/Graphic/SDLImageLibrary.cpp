#include "Graphic/__internalConfig/LoggerConfig.h"
#include "SDLImageLibrary.h"

#define SKA_IMG_LIB_NAME_DEFINE(ENUM, NAME) \
	SKA_LIB_NAME_DEFINE(ska::SDLImageIdNamedFunction, static_cast<int>(ska::SDLImageCalls::ENUM), NAME)

SKA_IMG_LIB_NAME_DEFINE(IMG_INIT, "IMG_Init");
SKA_IMG_LIB_NAME_DEFINE(IMG_LOAD, "IMG_Load");
SKA_IMG_LIB_NAME_DEFINE(IMG_QUIT, "IMG_Quit");

const ska::SDLImageLibrary& ska::SDLImageLibrary::get() {
	static SDLImageLibrary instance;
	return instance;
}
