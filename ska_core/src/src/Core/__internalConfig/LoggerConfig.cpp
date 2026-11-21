#include <iostream>
#include <fstream>
#include "LoggerConfig.h"

ska::SkaCoreLogger ska::GlobalCoreLogger = ska::BuildCoreLogger("Core.log.txt");

ska::SkaCoreLogger ska::BuildCoreLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaCoreLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    
    return logger;
}
