#include <iostream>
#include <sstream>
#include <benchmark/benchmark.h>
#include <Logging/MultiLogger.h>

static void BM_OutputDirectTest(benchmark::State& state) {
    std::stringstream ss;  
    for (auto _ : state) {
        benchmark::DoNotOptimize(ss << "test" << std::endl);
          //ss << "test" << std::endl;
      }
}

 static void BM_LogInfoTest(benchmark::State& state) {
     std::stringstream ss;
    using TestLogger = 
        ska::Logger<ska::LogLevel::Info, ska::LogLevel::Error, ska::LogSync>;
 
    auto logger = TestLogger{};
        logger.addOutputTarget(ss);
        logger.setPattern(ska::LogLevel::Info, "%v");

    for(auto _ : state) {
        SKA_LOGC_STATIC(logger, ska::LogLevel::Info, TestLogger) << "test";
    }
 }

static void BM_MultiLoggerInfoTest(benchmark::State& state) {
    using TestMultiLogger = ska::MultiLogger<
        ska::Logger<ska::LogLevel::Warn, ska::LogLevel::Error, ska::LogSync>, 
        ska::Logger<ska::LogLevel::Info, ska::LogLevel::Error, ska::LogSync>
    >;
    
    auto logger2 = TestMultiLogger{};
    std::stringstream ss;
    logger2.get<0>().addOutputTarget(ss);
    logger2.get<1>().addOutputTarget(ss);
    
    logger2.get<1>().setPattern(ska::LogLevel::Info, "%v");
    
    for(auto _ : state) {
        SKA_LOGC_STATIC(logger2, ska::LogLevel::Info, TestMultiLogger) << "test";
    }
 }

static void BM_LoggerErrorTest(benchmark::State& state) {
    using TestLoggerError = ska::Logger<ska::LogLevel::Error, ska::LogLevel::Error, ska::LogSync>;
    
    auto logger2 = TestLoggerError{};
    std::stringstream ss;
    logger2.addOutputTarget(ss);
    logger2.setPattern(ska::LogLevel::Info, "%v");
    
    for(auto _ : state) {
        SKA_LOGC_STATIC(logger2, ska::LogLevel::Warn, TestLoggerError) << "test";
    }
 }

struct ClassDisabledTest;
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ClassDisabledTest)

static void BM_LoggerClassErrorTest(benchmark::State& state) {
    using TestLoggerError = ska::Logger<ska::LogLevel::Info, ska::LogLevel::Error, ska::LogSync>;
    
    auto logger2 = TestLoggerError{};
    std::stringstream ss;
    logger2.addOutputTarget(ss);
    logger2.setPattern(ska::LogLevel::Info, "%v");
    
    for(auto _ : state) {
        SKA_LOGC_STATIC(logger2, ska::LogLevel::Warn, ClassDisabledTest) << "test";
    }
 }

BENCHMARK(BM_LoggerErrorTest);
BENCHMARK(BM_LoggerClassErrorTest);
BENCHMARK(BM_MultiLoggerInfoTest);
BENCHMARK(BM_LogInfoTest);
BENCHMARK(BM_OutputDirectTest);
BENCHMARK_MAIN();

