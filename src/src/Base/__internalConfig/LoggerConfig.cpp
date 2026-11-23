#include <iostream>
#include <fstream>
#include "Base/IO/Files/LogFilePath.h"
#include "LoggerConfig.h"

ska::SkaLogger ska::GlobalLogger = ska::BuildBaseLogger("Base.log.txt");

ska::SkaLogger ska::BuildBaseLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = ska::OpenLogFile(filename);
	auto logger = SkaLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);

    return logger;
}
