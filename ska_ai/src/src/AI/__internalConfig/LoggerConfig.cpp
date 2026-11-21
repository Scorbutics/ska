#include <iostream>
#include <fstream>
#include "LoggerConfig.h"

ska::SkaAiLogger ska::GlobalAiLogger = ska::BuildAiLogger("AI.log.txt");

ska::SkaAiLogger ska::BuildAiLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaAiLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    
    return logger;
}
