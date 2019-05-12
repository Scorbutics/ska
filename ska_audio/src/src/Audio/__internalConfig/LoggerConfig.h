#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	using SkaAudioLogger = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
	>;
	SkaAudioLogger BuildAudioLogger(const char* filename);

	extern SkaAudioLogger GlobalAudioLogger;
}
#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(ska::GlobalAudioLogger, level, currentClass)
#define SLOG(level) SKA_LOGC(ska::GlobalAudioLogger, level)
#endif
