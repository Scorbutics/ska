#include <iostream>
#include <fstream>
#include "Base/IO/Files/LogFilePath.h"
#include "LoggerConfigLang.h"

ska::detail::SkaLangLogger& ska::detail::LangLogger() {
	static auto logger = ska::detail::BuildLangLogger("Log.txt");
	return logger;
}

namespace ska {
	namespace detail {
		template<class T>
		void UpdatePatterns(T& logger) {
			logger.setPattern(ska::LogLevel::Debug, "(%m:%s)%10c[D]%8c(%25F l.%3l) %07c%v");
			logger.setPattern(ska::LogLevel::Info, "(%m:%s)%11c[I]%8c(%25F l.%3l) %07c%v");
			logger.setPattern(ska::LogLevel::Warn, "(%m:%s)%14c[W]%8c(%25F l.%3l) %07c%v");
			logger.setPattern(ska::LogLevel::Error, "(%m:%s)%12c[E]%8c(%25F l.%3l) %07c%v");
			logger.enableComplexLogging();
		}
	}
}
ska::detail::SkaLangLogger ska::detail::BuildLangLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = ska::OpenLogFile(filename);
	auto logger = SkaLangLogger{};
#ifndef NDEBUG
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);

	UpdatePatterns(logger.get<0>());
	UpdatePatterns(logger.get<1>());
#endif

	return logger;
}
