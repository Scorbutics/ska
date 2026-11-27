#pragma once
#include <cmath>

namespace ska {
	/**
	 * \brief Time of the drawing between two logic updates.
	 *
	 * The logic (systems, physics) runs at a fixed rate (GameCore), the drawing as fast as possible : between two logic
	 * updates, the drawing shows the moving things between their last two logic states (FrameClock::alpha, from 0 to 1),
	 * so they move smoothly. It is one logic update late (20 ms at 50 updates per second).
	 */
	class FrameClock {
	public:
		// Number of logic updates done
		static unsigned long tick();
		// Part of the next logic update elapsed when drawing, from 0 to 1
		static float alpha();

		// Called by the game loop
		static void logicUpdated();
		static void drawing(float alpha);

		// How the frames are paced (native : the browser paces them on the web) : by the display (V-sync, set by the
		// renderer), by a wait in the game loop (no V-sync : FallbackFrameTime ms a frame at least), not at all (SKA_VSYNC=0,
		// SKA_FIXED_STEP : as fast as possible)
		enum class Pacing { Display, Wait, None };
		static constexpr unsigned int FallbackFrameTime = 16;
		static void setPacing(Pacing pacing) { m_pacing = pacing; }
		static Pacing pacing() { return m_pacing; }
		// A frame was drawn at "now" (ms) : the frames per second are logged every 5 s (Core.log.txt, "Frames : ...")
		static void drawn(unsigned int now);

	private:
		static inline Pacing m_pacing = Pacing::Wait;
		static unsigned long m_tick;
		static float m_alpha;
	};

	/**
	 * \brief A value drawn between its last two logic states (FrameClock).
	 *
	 * "sample" is called when drawing, with the current value : the value changes at each logic update, the result moves
	 * from the previous logic value to the current one. A jump longer than "snapDistance" (a teleport) is not smoothed.
	 */
	class Interpolated {
	public:
		struct Value { float x = 0.F; float y = 0.F; float z = 0.F; };

		Value sample(const Value& current, float snapDistance) {
			const auto tick = FrameClock::tick();
			if (!m_initialized) {
				m_previous = current;
				m_current = current;
				m_tick = tick;
				m_initialized = true;
			} else if (tick != m_tick) {
				// Drawn again after a logic update : the last drawn logic state becomes the previous one
				m_previous = m_current;
				m_current = current;
				m_tick = tick;
			} else {
				// Changed outside the logic updates (ex : by a drawing system) : the change is shown right away
				const auto dx = current.x - m_current.x;
				const auto dy = current.y - m_current.y;
				const auto dz = current.z - m_current.z;
				m_previous.x += dx; m_previous.y += dy; m_previous.z += dz;
				m_current = current;
			}

			if (std::abs(m_current.x - m_previous.x) > snapDistance || std::abs(m_current.y - m_previous.y) > snapDistance || std::abs(m_current.z - m_previous.z) > snapDistance) {
				m_previous = m_current;
			}
			const auto alpha = FrameClock::alpha();
			return { m_previous.x + (m_current.x - m_previous.x) * alpha, m_previous.y + (m_current.y - m_previous.y) * alpha, m_previous.z + (m_current.z - m_previous.z) * alpha };
		}

	private:
		bool m_initialized = false;
		unsigned long m_tick = 0;
		Value m_previous;
		Value m_current;
	};
}
