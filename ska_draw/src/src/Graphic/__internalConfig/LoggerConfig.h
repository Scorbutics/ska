#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	using SkaDrawLogger = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
	>;
	SkaDrawLogger BuildDrawLogger(const char* filename);

	extern SkaDrawLogger GlobalDrawLogger;
}
#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(ska::GlobalDrawLogger, level, currentClass)
#define SLOG(level) SKA_LOGC(ska::GlobalDrawLogger, level)
#endif
