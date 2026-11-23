#pragma once

namespace ska {
	class TimeUtils {
	private:
		TimeUtils() = default;

	public:
		// Milliseconds since the start : the SDL clock, or the virtual clock when it is frozen (paused by the console, or
		// SKA_FIXED_STEP : every frame is one logic step, and the time moves only by these steps)
		static unsigned int getTicks();
		static void freeze();
		static void unfreeze();
		static bool frozen();
		// Moves the virtual clock (frozen)
		static void advance(unsigned int ms);
		// SKA_FIXED_STEP set in the environment
		static bool fixedStep();
		static void wait(unsigned int ms);
		~TimeUtils() = default;
	};
}
