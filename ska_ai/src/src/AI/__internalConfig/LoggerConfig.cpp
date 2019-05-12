#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaAiLogger ska::GlobalAiLogger = ska::BuildAiLogger("AI.log.txt");

ska::SkaAiLogger ska::BuildAiLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaAiLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalAiLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
