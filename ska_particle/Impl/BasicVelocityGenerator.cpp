#include "BasicVelocityGenerator.h"
#include "../ParticleFactory.h"
#include "../ParticleBuilder.h"

ska::BasicVelocityGenerator::BasicVelocityGenerator(PolarPoint<float> initialVelocityVector, float spreading, unsigned int slices, float randomMultiplier) :
	m_initialVelocityVector(initialVelocityVector),
	m_spreading(spreading),
	m_slices(slices),
	m_randomMultiplier(randomMultiplier) {
}

void ska::BasicVelocityGenerator::build(ParticleBuilder & builder) const {
	builder.setVelocity(m_initialVelocityVector, m_spreading, m_slices, m_randomMultiplier);
}
