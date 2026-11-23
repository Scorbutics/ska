#include "Core/__internalConfig/LoggerConfig.h"
#include <SDL3/SDL_timer.h>
#include "../SDLLibrary.h"
#include <cstdlib>
#include <string>
#include "TimeUtils.h"

namespace {
	struct VirtualClock {
		bool frozen = false;
		unsigned int ticks = 0;
	};
	VirtualClock& Clock() {
		static VirtualClock clock = [] {
			auto c = VirtualClock{};
			c.frozen = ska::TimeUtils::fixedStep();
			return c;
		}();
		return clock;
	}
}

bool ska::TimeUtils::fixedStep() {
	static const bool fixed = [] {
		const auto* value = std::getenv("SKA_FIXED_STEP");
		return value != nullptr && *value != '\0' && std::string{ value } != "0";
	}();
	return fixed;
}

unsigned int ska::TimeUtils::getTicks() {
	auto& clock = Clock();
	return clock.frozen ? clock.ticks : SDLLibrary::get().getTicks();
}

void ska::TimeUtils::freeze() {
	auto& clock = Clock();
	if (!clock.frozen) {
		clock.ticks = SDLLibrary::get().getTicks();
		clock.frozen = true;
	}
}

void ska::TimeUtils::unfreeze() {
	if (!fixedStep()) {
		Clock().frozen = false;
	}
}

bool ska::TimeUtils::frozen() {
	return Clock().frozen;
}

void ska::TimeUtils::advance(unsigned int ms) {
	Clock().ticks += ms;
}

void ska::TimeUtils::wait(unsigned int ms) {
    SDLLibrary::get().delay(static_cast<Uint32>(ms));
}
