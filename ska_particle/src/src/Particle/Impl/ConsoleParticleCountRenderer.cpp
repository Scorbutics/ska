#include <iostream>

#include "Particle/__internalConfig/LoggerConfig.h"
#include "ConsoleParticleCountRenderer.h"
#include "Core/Utils/TimeUtils.h"
#include "../ParticleGroup.h"


ska::ConsoleParticleCountRenderer::ConsoleParticleCountRenderer(unsigned int frequency) :
	m_frequency(frequency) {
}

void ska::ConsoleParticleCountRenderer::render(const Renderer&, const ParticleGroup& group) const {
	static unsigned int lastTime = ska::TimeUtils::getTicks();
	if(ska::TimeUtils::getTicks() - lastTime >= m_frequency) {
		std::cout << "Particle count : " << group.getLength() << std::endl;
		lastTime = ska::TimeUtils::getTicks();
	}
}
