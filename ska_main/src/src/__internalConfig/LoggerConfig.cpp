#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaMainLogger ska::GlobalMainLogger = ska::BuildMainLogger("Main.log.txt");

ska::SkaMainLogger ska::BuildMainLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaMainLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalMainLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
