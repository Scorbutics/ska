#include <SDL_timer.h>
#include "../LoggerConfig.h"
#include "../SDLLibrary.h"
#include "TimeUtils.h"

ska::TimeUtils::TimeUtils() {

}

unsigned int ska::TimeUtils::getTicks() {
	return SDLLibrary::get().getTicks();
}

void ska::TimeUtils::wait(unsigned int ms) {
    SDLLibrary::get().delay(static_cast<Uint32>(ms));
}

ska::TimeUtils::~TimeUtils() {

}
