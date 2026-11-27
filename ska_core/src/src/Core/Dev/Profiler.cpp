#include "Core/__internalConfig/LoggerConfig.h"
#include "Profiler.h"
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <map>
#include <sstream>
#include <typeindex>
#include <unordered_map>
#include <vector>
#if defined(__GNUG__)
#include <cxxabi.h>
#endif

namespace {
	struct ProfileState {
		bool enabled = false;
		long long slowMicros = 25000;
		std::unordered_map<std::string, long long> frame;
		std::unordered_map<std::string, long long> window;
		std::unordered_map<std::type_index, std::string> names;
		int frameSteps = 0;
		long long windowFrames = 0;
		long long windowMicros = 0;
		long long windowWorst = 0;
		long long windowSlow = 0;
		std::chrono::steady_clock::time_point windowStart = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point lastSlowLog{};
	};

	ProfileState& State() {
		static auto state = [] {
			auto s = ProfileState{};
			const auto* value = std::getenv("SKA_PROFILE");
			s.enabled = value != nullptr && *value != '\0' && std::string{ value } != "0";
			if (s.enabled) {
				const auto ms = std::atoi(value);
				if (ms > 1) {
					s.slowMicros = ms * 1000LL;
				}
			}
			return s;
		}();
		return state;
	}

	std::string Parts(const std::unordered_map<std::string, long long>& parts, double divider, std::size_t count) {
		auto sorted = std::vector<std::pair<std::string, long long>>{ parts.begin(), parts.end() };
		std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
		auto out = std::ostringstream{};
		out << std::fixed << std::setprecision(1);
		for (auto i = std::size_t{ 0 }; i < sorted.size() && i < count; i++) {
			out << (i == 0 ? "" : ", ") << sorted[i].first << " " << static_cast<double>(sorted[i].second) / 1000.0 / divider;
		}
		return out.str();
	}
}

bool ska::Profiler::enabled() {
	return State().enabled;
}

const std::string& ska::Profiler::name(const std::type_info& type) {
	auto& names = State().names;
	const auto key = std::type_index{ type };
	auto found = names.find(key);
	if (found == names.end()) {
		auto readable = std::string{ type.name() };
#if defined(__GNUG__)
		auto status = 0;
		if (auto* demangled = abi::__cxa_demangle(type.name(), nullptr, nullptr, &status); demangled != nullptr) {
			readable = demangled;
			std::free(demangled);
		}
#endif
		// Without its namespace
		if (const auto colon = readable.rfind("::"); colon != std::string::npos && readable.find('<') == std::string::npos) {
			readable = readable.substr(colon + 2);
		}
		found = names.emplace(key, readable).first;
	}
	return found->second;
}

void ska::Profiler::add(const std::string& part, long long micros) {
	State().frame[part] += micros;
}

void ska::Profiler::logicStep() {
	if (enabled()) {
		State().frameSteps++;
	}
}

void ska::Profiler::frame(long long micros) {
	auto& s = State();
	if (!s.enabled) {
		return;
	}
	const auto now = std::chrono::steady_clock::now();
	if (micros >= s.slowMicros) {
		s.windowSlow++;
		// At most 4 slow frames a second in the log
		if (now - s.lastSlowLog >= std::chrono::milliseconds(250)) {
			s.lastSlowLog = now;
			SLOG_STATIC(LogLevel::Info, Profiler) << "Profile : slow frame " << std::fixed << std::setprecision(1) << static_cast<double>(micros) / 1000.0
				<< " ms (" << s.frameSteps << " logic steps) : " << Parts(s.frame, 1.0, 10);
		}
	}
	for (const auto& [part, time] : s.frame) {
		s.window[part] += time;
	}
	s.windowFrames++;
	s.windowMicros += micros;
	s.windowWorst = std::max(s.windowWorst, micros);
	s.frame.clear();
	s.frameSteps = 0;
	if (now - s.windowStart >= std::chrono::seconds(5) && s.windowFrames > 0) {
		const auto frames = static_cast<double>(s.windowFrames);
		SLOG_STATIC(LogLevel::Info, Profiler) << "Profile : 5 s, " << s.windowFrames << " frames, " << std::fixed << std::setprecision(1)
			<< static_cast<double>(s.windowMicros) / 1000.0 / frames << " ms a frame, the worst " << static_cast<double>(s.windowWorst) / 1000.0
			<< " ms, " << s.windowSlow << " slow ; by frame (ms) : " << Parts(s.window, frames, 14);
		s.window.clear();
		s.windowFrames = 0;
		s.windowMicros = 0;
		s.windowWorst = 0;
		s.windowSlow = 0;
		s.windowStart = now;
	}
}
