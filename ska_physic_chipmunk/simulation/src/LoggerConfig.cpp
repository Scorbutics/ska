#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaPhysicLogger ska::GlobalPhysicLogger = ska::BuildPhysicLogger("Core.log.txt");

ska::SkaPhysicLogger ska::BuildPhysicLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaPhysicLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalPhysicLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
