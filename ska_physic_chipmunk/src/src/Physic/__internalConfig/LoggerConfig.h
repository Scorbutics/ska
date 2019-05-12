#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	using SkaPhysicLogger = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
	>;
	SkaPhysicLogger BuildPhysicLogger(const char* filename);

	extern SkaPhysicLogger GlobalPhysicLogger;
}
#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(ska::GlobalPhysicLogger, level, currentClass)
#define SLOG(level) SKA_LOGC(ska::GlobalPhysicLogger, level)
#endif
