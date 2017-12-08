#include "ParticleEffect.h"
#include "ParticleUpdater.h"
#include "ParticleRenderer.h"
#include "ParticleGenerator.h"

ska::ParticleEffect::ParticleEffect(unsigned int density, std::size_t systemSize) :
    m_density(density),
    m_factory(m_builder),
	m_group(systemSize) {
}

void ska::ParticleEffect::generate(unsigned int dt) {
    auto& builder = m_factory.prepareNextParticles(m_group, m_density, dt);
    if(!builder.isEmpty()) {
        for (const auto& e : m_generators) {
            e->build(builder);
        }
    }
}

void ska::ParticleEffect::update(unsigned int dt) {
    for(const auto& u : m_updaters) {
		u->update(dt, m_group);
	}
    m_factory.updateCurrentActiveCounter();
}

void ska::ParticleEffect::refresh(unsigned int dt) {
    generate(dt);
	update(dt);
}

void ska::ParticleEffect::render(const Renderer& renderer) const {
	for (const auto& u : m_renderers) {
		u->render(renderer, m_group);
	}
}
