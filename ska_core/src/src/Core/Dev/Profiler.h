#pragma once
#include <chrono>
#include <string>
#include <typeinfo>

namespace ska {
	/**
	 * \brief Where the time of the frames goes (SKA_PROFILE=1, or SKA_PROFILE=<ms> : the threshold of a slow frame, 25 ms
	 * by default). Each logic system, each graphic system, each drawing (by its type), the draw and the present are timed :
	 * a slow frame is logged with its parts, the slowest first (Core.log.txt, "Profile : slow frame ..."), and every 5 s
	 * the average of each part by frame ("Profile : 5 s ..."). Without SKA_PROFILE : nothing is measured.
	 */
	class Profiler {
	public:
		static bool enabled();
		// The name of a type (a system, a drawing), readable
		static const std::string& name(const std::type_info& type);
		// A part of the frame took "micros"
		static void add(const std::string& part, long long micros);
		// A logic step was done in this frame
		static void logicStep();
		// The frame is over (its time, from its start)
		static void frame(long long micros);

		// Times its scope as a part of the frame (nothing without SKA_PROFILE)
		class Scope {
		public:
			explicit Scope(const std::string& part) : m_part(enabled() ? &part : nullptr) {
				if (m_part != nullptr) {
					m_start = std::chrono::steady_clock::now();
				}
			}
			~Scope() {
				if (m_part != nullptr) {
					add(*m_part, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_start).count());
				}
			}
			Scope(const Scope&) = delete;
			Scope& operator=(const Scope&) = delete;
		private:
			const std::string* m_part;
			std::chrono::steady_clock::time_point m_start;
		};
	};
}
