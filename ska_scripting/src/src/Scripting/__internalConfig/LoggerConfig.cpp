#include <iostream>
#include <fstream>
#include "LoggerConfig.h"

ska::SkaScriptingLogger ska::GlobalScriptingLogger = ska::BuildScriptingLogger("Scripting.log.txt");

ska::SkaScriptingLogger ska::BuildScriptingLogger(const char* filename) {
	static auto ScriptingLogFileOutput = std::ofstream { filename };
	auto logger = SkaScriptingLogger{};
	logger.get<0>().addOutputTarget(ScriptingLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
	return logger;
}
