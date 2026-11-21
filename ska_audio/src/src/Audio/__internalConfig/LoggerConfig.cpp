#include <iostream>
#include <fstream>
#include "LoggerConfig.h"

ska::SkaAudioLogger ska::GlobalAudioLogger = ska::BuildAudioLogger("Audio.log.txt");

ska::SkaAudioLogger ska::BuildAudioLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaAudioLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    
    return logger;
}
