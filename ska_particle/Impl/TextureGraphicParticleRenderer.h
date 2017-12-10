#pragma once
#include "../ParticleRenderer.h"

namespace ska {
	class TextureGraphicParticleRenderer :
		public ParticleRenderer {
	public:
		explicit TextureGraphicParticleRenderer(Renderer& nativeRenderer);
		virtual ~TextureGraphicParticleRenderer() = default;

		void render(const Renderer& renderer, const ParticleGroup& group) const override;
	private:
		Renderer& m_renderer;
	};
}
