#pragma once
#include "../ParticleRenderer.h"

namespace ska {
	class Renderer;

	class ColoredRectGraphicParticleRenderer :
		public ParticleRenderer {
	public:
		explicit ColoredRectGraphicParticleRenderer(Renderer& renderer, unsigned int particleDimensions);
		virtual ~ColoredRectGraphicParticleRenderer() = default;

		void render(const Renderer& renderer, const ParticleGroup& group) const override;

	private:
		Renderer& m_renderer;
		unsigned int m_particleDimensions;
	};
}
