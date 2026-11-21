#include "Core/__internalConfig/LoggerConfig.h"
#include <SDL3/SDL_timer.h>
#include "../SDLLibrary.h"
#include "TimeUtils.h"

unsigned int ska::TimeUtils::getTicks() {
	return SDLLibrary::get().getTicks();
}

void ska::TimeUtils::wait(unsigned int ms) {
    SDLLibrary::get().delay(static_cast<Uint32>(ms));
}
