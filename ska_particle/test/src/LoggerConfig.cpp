#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaParticleLogger ska::GlobalParticleLogger = ska::BuildParticleLogger("Particle.log.txt");

ska::SkaParticleLogger ska::BuildParticleLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaParticleLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalParticleLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
