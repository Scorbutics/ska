#include <iostream>
#include <fstream>
#include "LoggerConfig.h"

ska::SkaMainLogger ska::GlobalMainLogger = ska::BuildMainLogger("Main.log.txt");

ska::SkaMainLogger ska::BuildMainLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaMainLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    
    return logger;
}
