#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	using SkaMainLogger = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
	>;
	SkaMainLogger BuildMainLogger(const char* filename);

	extern SkaMainLogger GlobalMainLogger;
}
#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(ska::GlobalMainLogger, level, currentClass)
#define SLOG(level) SKA_LOGC(ska::GlobalMainLogger, level)
#endif
