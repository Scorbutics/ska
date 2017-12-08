#pragma once
#include "../ParticleRenderer.h"
#include "Graphic/Texture.h"

namespace ska {
	class TextureGraphicParticleRenderer :
		public ParticleRenderer {
	public:
		explicit TextureGraphicParticleRenderer(Renderer& nativeRenderer);
		virtual ~TextureGraphicParticleRenderer() = default;

		void display(const ParticleGroup& group) const override;
	private:
		Renderer& m_renderer;
	};
}
