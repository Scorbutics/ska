#pragma once
#include "../ParticleEffect.h"

namespace ska {
	struct SpreadingParticleSystemData;

	class SpreadingColorParticleEffectFactory {
	public:
		~SpreadingColorParticleEffectFactory() = default;
		static std::unique_ptr<ParticleEffect> createEffect(Renderer& renderer, const SpreadingParticleSystemData& data);
		
	private:
		SpreadingColorParticleEffectFactory() = default;
	};
}
