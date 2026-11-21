#include "FrameClock.h"

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
