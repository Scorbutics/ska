#pragma once

namespace ska {
	class ParticleGroup;
	class Renderer;

	class ParticleRenderer {
	protected:
		ParticleRenderer() = default;
	public:
		virtual void render(const Renderer& renderer, const ParticleGroup& group) const = 0;
		virtual ~ParticleRenderer() = default;
	};
}