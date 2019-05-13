#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaScriptLogger ska::GlobalScriptLogger = ska::BuildScriptLogger("Script.log.txt");

ska::SkaScriptLogger ska::BuildScriptLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaScriptLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalScriptLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
