#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	using SkaLogger = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
	>;
	SkaLogger BuildLogger(const char* filename);

	extern SkaLogger GlobalLogger;
}
#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(ska::GlobalLogger, level, currentClass)
#define SLOG(level) SKA_LOGC(ska::GlobalLogger, level)
#endif
