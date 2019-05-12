#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaDrawLogger ska::GlobalDrawLogger = ska::BuildDrawLogger("Draw.log.txt");

ska::SkaDrawLogger ska::BuildDrawLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaDrawLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalDrawLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
