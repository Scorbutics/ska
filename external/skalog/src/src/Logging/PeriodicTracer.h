#pragma once

#include <sstream>
#include "LogLevel.h"

namespace ska {
	struct PeriodicTracerBinding {
		uint32_t counter = 0;
	};

	template <class Logger, LogLevel Level>
	class PeriodicTracer {
	public:
		PeriodicTracer(Logger& logger) :
			m_logger(&logger) {
		}
		~PeriodicTracer() = default;

		template <class Target, size_t Line>
		void trace(const Target& target, uint32_t frequency, const char* function, const char* file) {
			// Per target, per line binding (because of the templated "Line" parameter + static)
			static PeriodicTracerBinding binding = { Line };

			if (++binding.counter % (frequency + 1) == 0) { 
				m_logger->template log<Level, Target, Line>(function, file) << ToString(target);
				binding.counter = Line;
			}
		}

	private:
		template <class Target>
		static std::string ToString(const Target& t) {
			auto ss = std::stringstream {};
			ss << t;
			return ss.str();
		}
		Logger* m_logger;
	};
}

#ifndef SKA_LOG_DONT_USE_LOG_DEFINE
#define SKA_LOG_PERIODIC_TRACE(frequency) trace<std::decay_t<decltype(*this)>, __LINE__>(*this, frequency, SKA_CURRENT_FUNCTION, __FILE__)
#endif