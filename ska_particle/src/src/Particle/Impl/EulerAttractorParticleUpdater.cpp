#include "Particle/__internalConfig/LoggerConfig.h"
#include "EulerAttractorParticleUpdater.h"
#include "../ParticleGroup.h"

ska::EulerAttractorParticleUpdater::EulerAttractorParticleUpdater(Point<int>& origin, PolarPoint<float>& forceVector) :
	m_origin(origin) {
	m_force = forceVector;
}

void ska::EulerAttractorParticleUpdater::update(unsigned int, ParticleGroup & group) {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		const auto diff = Point<long>{ static_cast<long>(m_origin.x - group.pos[i].x), static_cast<long>(m_origin.y - group.pos[i].y) };
		m_force.angle = static_cast<float>(NumberUtils::arctan(static_cast<float>(diff.x), static_cast<float>(diff.y)));
		group.physics[i].acceleration += ska::Point<float>::cartesian(m_force.radius / static_cast<float>(ska::NumberUtils::squareroot(static_cast<double>(diff.x) * diff.x + diff.y * diff.y)), m_force.angle);
	}
}

void ska::EulerAttractorParticleUpdater::move(const Point<int>& point) {
	m_origin = point;
}


