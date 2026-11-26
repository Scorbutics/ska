#include <iostream>
#include <fstream>
#include "LoggerSerializer.h"

#define SKALANG_SERIALIZER_LOGGING

ska::detail::SkaLangSerializerLogger& ska::detail::LangSerializerLogger() {
	static auto logger = ska::detail::BuildLangSerializerLogger("SerializerLog.log");
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
ska::detail::SkaLangSerializerLogger ska::detail::BuildLangSerializerLogger(const char * filename) {
	static auto LogFileOutput = std::ofstream { filename };
	auto logger = SkaLangSerializerLogger{};
#ifdef SKALANG_SERIALIZER_LOGGING
	logger.get<0>().addOutputTarget(LogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);

	UpdatePatterns(logger.get<0>());
	UpdatePatterns(logger.get<1>());
#endif
	return logger;
}
