#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	using SkaScriptingLogger = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Info, ska::LogLevel::Error, ska::LogSync>
	>;
	SkaScriptingLogger BuildScriptingLogger(const char* filename);

	extern SkaScriptingLogger GlobalScriptingLogger;
}

#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(ska::GlobalScriptingLogger, level, currentClass)
#define SLOG(level) SKA_LOGC(ska::GlobalScriptingLogger, level)
#endif
