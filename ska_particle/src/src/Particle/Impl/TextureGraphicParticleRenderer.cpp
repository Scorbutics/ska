#include "Particle/__internalConfig/LoggerConfig.h"
#include "TextureGraphicParticleRenderer.h"
#include "../ParticleGroup.h"
#include "Core/Draw/Renderer.h"

ska::TextureGraphicParticleRenderer::TextureGraphicParticleRenderer(Renderer& nativeRenderer) : m_renderer(nativeRenderer) {
}

void ska::TextureGraphicParticleRenderer::render(const Renderer&, const ParticleGroup& group) const {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		m_renderer.render(group.appearance, static_cast<int>(group.pos[i].x), static_cast<int>(group.pos[i].y), nullptr);
	}
}
