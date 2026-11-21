#include <iostream>
#include <fstream>
#include "LoggerConfig.h"

ska::SkaPhysicLogger ska::GlobalPhysicLogger = ska::BuildPhysicLogger("Core.log.txt");

ska::SkaPhysicLogger ska::BuildPhysicLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaPhysicLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    
    return logger;
}
