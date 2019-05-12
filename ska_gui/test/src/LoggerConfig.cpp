#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaGuiLogger ska::GlobalGuiLogger = ska::BuildGuiLogger("Gui.log.txt");

ska::SkaGuiLogger ska::BuildGuiLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaGuiLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalGuiLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
