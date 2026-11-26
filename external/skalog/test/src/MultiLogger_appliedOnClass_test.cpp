#include <iostream>
#include <doctest.h>
#include <Logging/MultiLogger.h>

struct MultiLoggerTestErrorOnly;
struct MultiLoggerTestDisabled;

SKA_LOGC_CONFIG(ska::LogLevel::Error, MultiLoggerTestErrorOnly)
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, MultiLoggerTestDisabled)

TEST_CASE("[MultiLogger] Sync - Compile time Full range log levels - Class dependant tests") {
	
	auto ss = std::stringstream {};
	auto logger = ska::MultiLogger<ska::Logger<>, ska::Logger<>>{};
	logger.get<0>().addOutputTarget(ss);
	logger.get<1>().addOutputTarget(ss);

	logger.get<0>().setPattern(ska::LogLevel::Debug, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Info, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Warn, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Error, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Debug, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Info, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Warn, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Error, "%v");
	
	SUBCASE("Error only for the \"MultiLoggerTestErrorOnly\" class") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Info, MultiLoggerTestErrorOnly) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, MultiLoggerTestErrorOnly) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, MultiLoggerTestErrorOnly) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Error, MultiLoggerTestErrorOnly) << "test";
		CHECK(ss.str() == "test\ntest\n");
	}

	SUBCASE("Disabled for the \"MultiLoggerTestDisabled\" class") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Info, MultiLoggerTestDisabled) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, MultiLoggerTestDisabled) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, MultiLoggerTestDisabled) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Error, MultiLoggerTestDisabled) << "test";
		CHECK(ss.str().empty());
	}	
}
