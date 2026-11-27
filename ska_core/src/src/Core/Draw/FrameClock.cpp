#include "Core/__internalConfig/LoggerConfig.h"
#include "FrameClock.h"

void ska::FrameClock::drawn(unsigned int now) {
	static auto since = now;
	static auto frames = 0U;
	frames++;
	if (now - since >= 5000) {
		SLOG_STATIC(ska::LogLevel::Info, ska::FrameClock) << "Frames : " << (frames * 1000U / (now - since)) << " per second";
		since = now;
		frames = 0;
	}
}

unsigned long ska::FrameClock::m_tick = 0;
float ska::FrameClock::m_alpha = 1.F;

unsigned long ska::FrameClock::tick() {
	return m_tick;
}

float ska::FrameClock::alpha() {
	return m_alpha;
}

void ska::FrameClock::logicUpdated() {
	m_tick++;
}

void ska::FrameClock::drawing(float alpha) {
	m_alpha = alpha < 0.F ? 0.F : (alpha > 1.F ? 1.F : alpha);
}
