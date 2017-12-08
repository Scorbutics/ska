#include "TextureGraphicParticleRenderer.h"
#include "../ParticleGroup.h"
#include "Draw/Renderer.h"

ska::TextureGraphicParticleRenderer::TextureGraphicParticleRenderer(Renderer& nativeRenderer) : m_renderer(nativeRenderer) {
}

void ska::TextureGraphicParticleRenderer::render(const Renderer& nativeRenderer, const ParticleGroup& group) const {
	const auto& maxSize = group.getLength();
	for (std::size_t i = 0; i < maxSize; i++) {
		m_renderer.render(group.appearance, group.pos[i].x, group.pos[i].y);
	}
}
