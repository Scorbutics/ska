#include "SpreadingTextureParticleEffectFactory.h"
#include "SpreadingParticleEffectFactory.h"
#include "TextureGraphicParticleRenderer.h"
#include "SpritePathProperty.h"
#include "GenerationDelayProperty.h"
#include "TextureAlphaParticleUpdater.h"

std::unique_ptr<ska::ParticleEffect> ska::SpreadingTextureParticleEffectFactory::createEffect(Renderer& renderer, const SpreadingParticleSystemData & data) {
	auto particleSystem = SpreadingParticleEffectFactory::createEffect(renderer, data);
	particleSystem->applyGroupProperty<SpritePathProperty>(data.spritePath);
	particleSystem->applyGroupProperty<GenerationDelayProperty>(data.generationDelay);

	particleSystem->addUpdater<TextureAlphaParticleUpdater>(data.lifetime);
	particleSystem->addRenderer<TextureGraphicParticleRenderer>(static_cast<Renderer&>(renderer));
	return std::move(particleSystem);
}
