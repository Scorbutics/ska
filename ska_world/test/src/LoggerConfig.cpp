#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaWorldLogger ska::GlobalWorldLogger = ska::BuildWorldLogger("World.log.txt");

ska::SkaWorldLogger ska::BuildWorldLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaWorldLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalWorldLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
