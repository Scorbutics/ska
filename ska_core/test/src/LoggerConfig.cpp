#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaCoreLogger ska::GlobalCoreLogger = ska::BuildCoreLogger("Core.log.txt");

ska::SkaCoreLogger ska::BuildCoreLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaCoreLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalCoreLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
