#include <iostream>
#include <doctest.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

TEST_CASE("[Logger] Sync - Compile time Full range log levels") {
	
	using LoggerTest = ska::Logger<>;
	auto ss = std::stringstream{};
	auto logger = LoggerTest{};
	logger.addOutputTarget(ss);
	
	CHECK(LoggerTest::accept<ska::LogLevel::Debug>());
	CHECK(LoggerTest::accept<ska::LogLevel::Info>());
	CHECK(LoggerTest::accept<ska::LogLevel::Warn>());
	CHECK(LoggerTest::accept<ska::LogLevel::Error>());

	SUBCASE("Basic pattern (value only) on Log Info") {
		logger.setPattern(ska::LogLevel::Info, "%v");

		SUBCASE("1 Log info") {
			logger.log<ska::LogLevel::Info, LoggerTest, __LINE__>(SKA_CURRENT_FUNCTION, __FILE__) << "test";
			CHECK(ss.str() == "test\n");
		}

		SUBCASE("4 Log info") {
			logger.log<ska::LogLevel::Info, LoggerTest, __LINE__>(SKA_CURRENT_FUNCTION, __FILE__) << "test" << " with " << 4 << " entries ?";
			CHECK(ss.str() == "test with 4 entries ?\n");
		}

		SUBCASE("1 Log != info") {
			logger.log<ska::LogLevel::Debug, LoggerTest, __LINE__>(SKA_CURRENT_FUNCTION, __FILE__) << "test";
			CHECK(ss.str() != "test\n");
		}

	}

	//Ensures we have 4 differents Tokenizer in a logger (one for each log level)
	SUBCASE("Different patterns test") {
		logger.setPattern(ska::LogLevel::Debug, "%v 1");
		logger.setPattern(ska::LogLevel::Info, "%v 2");
		logger.setPattern(ska::LogLevel::Warn, "%v 3");
		logger.setPattern(ska::LogLevel::Error, "%v 4");

		SUBCASE("1 Log Debug") {
			SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
			CHECK(ss.str() == "test 1\n");
		}

		SUBCASE("1 Log Info") {
			SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTest) << "test";
			CHECK(ss.str() == "test 2\n");
		}

		SUBCASE("1 Log Warn") {
			SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTest) << "test";
			CHECK(ss.str() == "test 3\n");
		}

		SUBCASE("1 Log Error") {
			SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "test";
			CHECK(ss.str() == "test 4\n");
		}
	}

	//We are not testing the Tokenizer part, so we set the output pattern at value only
	SUBCASE("Basic pattern : value only") {
		logger.setPattern(ska::LogLevel::Debug, "%v");
		logger.setPattern(ska::LogLevel::Info, "%v");
		logger.setPattern(ska::LogLevel::Warn, "%v");
		logger.setPattern(ska::LogLevel::Error, "%v");

		SUBCASE("Runtime set : only logs >= Info") {
			logger.configureLogLevel(ska::LogLevel::Info);

			SUBCASE("1 Log Debug") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
				CHECK(ss.str().empty());
			}

			SUBCASE("1 Log Info") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTest) << "test";
				CHECK(ss.str() == "test\n");
			}

			SUBCASE("1 Log Warn") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTest) << "test";
				CHECK(ss.str() == "test\n");
			}

			SUBCASE("1 Log Error") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "test";
				CHECK(ss.str() == "test\n");
			}
		}

		SUBCASE("Runtime set : only logs = Error") {
			logger.configureLogLevel(ska::LogLevel::Error);

			SUBCASE("1 Log Debug") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
				CHECK(ss.str().empty());
			}

			SUBCASE("1 Log Info") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTest) << "test";
				CHECK(ss.str().empty());
			}

			SUBCASE("1 Log Warn") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTest) << "test";
				CHECK(ss.str().empty());
			}

			SUBCASE("1 Log Error") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "test";
				CHECK(ss.str() == "test\n");
			}
		}

		SUBCASE("Runtime set : only logs = Disabled") {
			logger.configureLogLevel(ska::LogLevel::Disabled);

			SUBCASE("1 Log Debug") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
				CHECK(ss.str().empty());
			}

			SUBCASE("1 Log Info") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTest) << "test";
				CHECK(ss.str().empty());
			}

			SUBCASE("1 Log Warn") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTest) << "test";
				CHECK(ss.str().empty());
			}

			SUBCASE("1 Log Error") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "test";
				CHECK(ss.str().empty());
			}

		}

		SUBCASE("2 output target") {
			auto ss2 = std::stringstream{};
			SUBCASE("Filtering logs on content") {
				logger.addOutputTarget(ss2, [](const ska::LogEntry& entry) {
					return entry.getMessage().find("test") != std::string::npos;
				});

				SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
				SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "tututi";
				CHECK(ss.str() == "test\ntututi\n");
				CHECK(ss2.str() == "test\n");
			}

			SUBCASE("Filtering logs on level (runtime)") {
				logger.addOutputTarget(ss2, [](const ska::LogEntry& entry) {
					return entry.getContext().logLevel < ska::LogLevel::Error;
				});

				SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
				SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "tututi";
				CHECK(ss.str() == "test\ntututi\n");
				CHECK(ss2.str() == "test\n");
			}

			SUBCASE("Copy") {
				logger.addOutputTarget(ss2);

				SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
				SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "tututi";
				CHECK(ss.str() == "test\ntututi\n");
				CHECK(ss2.str() == "test\ntututi\n");
			}
		}
	}

	SUBCASE("Complex pattern (recursive pattern contained by log message)") {
		logger.setPattern(ska::LogLevel::Debug, "%v");
		logger.setPattern(ska::LogLevel::Info, "%v");
		logger.setPattern(ska::LogLevel::Warn, "%v");
		logger.setPattern(ska::LogLevel::Error, "%v");

		SUBCASE("Disabled functionality") {
			SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "test %6F";
			CHECK(ss.str() == "test %6F\n");
		}

		SUBCASE("Enabled functionality") {
			logger.enableComplexLogging();

			SUBCASE("with File mention") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "test %6F";
				CHECK(ss.str() == "test Logger\n");
			}

			SUBCASE("protection against infinite recursion") {
				SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "test %v";
				CHECK(ss.str() == "test \n");
			}
		}
	}
	
}

TEST_CASE("[Logger] Sync - Compile time limited range log level") {
	
	using LoggerTest = ska::Logger<ska::LogLevel::Warn, ska::LogLevel::Error, ska::LogSync>;
	auto ss = std::stringstream{};
	auto logger = LoggerTest{};
	logger.addOutputTarget(ss);
	
	logger.setPattern(ska::LogLevel::Debug, "%v");
	logger.setPattern(ska::LogLevel::Info, "%v");
	logger.setPattern(ska::LogLevel::Warn, "%v");
	logger.setPattern(ska::LogLevel::Error, "%v");

	SUBCASE("Log Debug") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
		CHECK(ss.str().empty());
	}
	
	SUBCASE("Log Info") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Info, LoggerTest) << "test";
		CHECK(ss.str().empty());
	}

	SUBCASE("Log Warn") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Warn, LoggerTest) << "test";
		CHECK(ss.str() == "test\n");
	}

	SUBCASE("Log Error") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Error, LoggerTest) << "test";
		CHECK(ss.str() == "test\n");
	}
}

TEST_CASE("[Logger] Async - Basic tests") {
	using LoggerTest = ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>;
	auto ss = std::stringstream {};
	auto logger = LoggerTest{};
	logger.addOutputTarget(ss);

	logger.setPattern(ska::LogLevel::Debug, "%v");
	logger.setPattern(ska::LogLevel::Info, "%v");
	logger.setPattern(ska::LogLevel::Warn, "%v");
	logger.setPattern(ska::LogLevel::Error, "%v");

	SUBCASE("Log Debug") {
		SKA_LOGC_STATIC(logger, ska::LogLevel::Debug, LoggerTest) << "test";
		logger.terminate();
		CHECK(ss.str() == "test\n");
	}
}

