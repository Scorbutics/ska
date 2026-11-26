#include <doctest.h>
#include <Logging/LogAsync.h>
#include <Logging/MultiLogger.h>

#include <chrono>
#include <thread>

TEST_CASE("[MultiLogger] Sync & Sync - Basic tests") {
	using LoggerTest = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Warn, ska::LogLevel::Error, ska::LogSync>,
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
	>;

	auto firstOutput = std::stringstream {};
	auto secondOutput = std::stringstream{};

	auto logger = LoggerTest{};
	logger.get<0>().setPattern(ska::LogLevel::Debug, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Info, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Warn, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Error, "%v");

	logger.get<1>().setPattern(ska::LogLevel::Debug, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Info, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Warn, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Error, "%v");

	SUBCASE("Output basic text") {
		logger.get<0>().addOutputTarget(firstOutput);
		logger.get<1>().addOutputTarget(secondOutput);
		SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTest) << "test";
		CHECK(firstOutput.str() == "test\n");
		CHECK(secondOutput.str() == "test\n");
	}

	SUBCASE("Same output") {
		logger.get<0>().addOutputTarget(firstOutput);
		logger.get<1>().addOutputTarget(firstOutput);
		SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTest) << "test";
		CHECK(firstOutput.str() == "test\ntest\n");
		CHECK(secondOutput.str().empty());
	}

	SUBCASE("Low log level") {
		logger.get<0>().addOutputTarget(firstOutput);
		logger.get<1>().addOutputTarget(secondOutput);
		SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTest) << "test";
		CHECK(firstOutput.str().empty());
		CHECK(secondOutput.str() == "test\n");
	}
}

TEST_CASE("[MultiLogger] Sync & Async - Basic tests (multithreaded env)") {
	using LoggerTest = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
	>;

	auto firstOutput = std::stringstream{};
	auto secondOutput = std::stringstream{};

	auto logger = LoggerTest{};
	logger.get<0>().setPattern(ska::LogLevel::Debug, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Info, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Warn, "%v");
	logger.get<0>().setPattern(ska::LogLevel::Error, "%v");

	logger.get<1>().setPattern(ska::LogLevel::Debug, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Info, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Warn, "%v");
	logger.get<1>().setPattern(ska::LogLevel::Error, "%v");



	SUBCASE("Output basic text") {
		logger.get<0>().addOutputTarget(firstOutput);
		logger.get<1>().addOutputTarget(secondOutput);

		SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTest) << "test";
		logger.terminate();
		CHECK(firstOutput.str() == "test\n");
		CHECK(secondOutput.str() == "test\n");
	}

	SUBCASE("Same output (with multithread : cannot guaranty thread safety while writing in \"firstOutput\")") {
		logger.get<0>().addOutputTarget(firstOutput);
		logger.get<1>().addOutputTarget(firstOutput);

		SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTest) << "test" << " tututu" << 1 <<  " tatata !";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		logger.terminate();
		
		CHECK(!firstOutput.str().empty());
		CHECK(secondOutput.str().empty());
	}
}