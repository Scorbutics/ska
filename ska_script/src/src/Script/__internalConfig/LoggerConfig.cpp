#include <iostream>
#include <fstream>
#include "Base/IO/Files/LogFilePath.h"
#include "LoggerConfig.h"

ska::SkaScriptLogger ska::GlobalScriptLogger = ska::BuildScriptLogger("Script.log.txt");

ska::SkaScriptLogger ska::BuildScriptLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = ska::OpenLogFile(filename);
	auto logger = SkaScriptLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    
    return logger;
}
