#include <iostream>
#include <doctest.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

struct LoggerTestErrorOnly;
struct LoggerTestDisabled;

SKA_LOGC_CONFIG(ska::LogLevel::Error, LoggerTestErrorOnly)
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, LoggerTestDisabled)

TEST_CASE("[Logger] Sync - Compile time Full range log levels - Class dependant tests") {
	
	auto ss = std::stringstream {};
	auto logger = ska::Logger<>{};
	logger.addOutputTarget(ss);

	logger.setPattern(ska::LogLevel::Warn, "%v");
	logger.setPattern(ska::LogLevel::Error, "%v");
	
	SUBCASE("Error only for the \"LoggerTestErrorOnly\" class") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTestErrorOnly) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTestErrorOnly) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTestErrorOnly) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTestErrorOnly) << "test";
		CHECK(ss.str() == "test\n");
	}

	SUBCASE("Disabled for the \"LoggerTestDisabled\" class") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTestDisabled) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTestDisabled) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTestDisabled) << "test";
		SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTestDisabled) << "test";
		CHECK(ss.str().empty());
	}	
}
