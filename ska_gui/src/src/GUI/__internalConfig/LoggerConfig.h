#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	using SkaGuiLogger = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
	>;
	SkaGuiLogger BuildGuiLogger(const char* filename);

	extern SkaGuiLogger GlobalGuiLogger;
}
#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(ska::GlobalGuiLogger, level, currentClass)
#define SLOG(level) SKA_LOGC(ska::GlobalGuiLogger, level)
#endif
