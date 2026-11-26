#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	namespace detail {
		using SkaLangSerializerLogger = ska::MultiLogger<
			ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
			ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
		>;
		SkaLangSerializerLogger BuildLangSerializerLogger(const char* filename);

		SkaLangSerializerLogger& LangSerializerLogger();
	}
}
#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(ska::detail::LangSerializerLogger(), level, currentClass)
#define SLOG(level) SKA_LOGC(ska::detail::LangSerializerLogger(), level)
#endif
