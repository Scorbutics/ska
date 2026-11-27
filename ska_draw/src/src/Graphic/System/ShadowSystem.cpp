#include "Graphic/__internalConfig/LoggerConfig.h"
#include "ShadowSystem.h"
#include <algorithm>
#include <cmath>
#include "Core/Utils/SpritePath.h"
#include "CameraSystem.h"
#include "Core/Draw/DrawableContainer.h"
#include "../PositionnedGraphicDrawable.h"
#include "Core/Utils/SkaConstants.h"

namespace {
	// The pixels of its bottom hidden (GraphicComponent::cropBottom) from which its shadow is not drawn
	constexpr int ShadowCropFade = 12;
	// The height (pixels) from which its shadow fades, and from which it is not drawn
	constexpr long ShadowAirStart = 24;
	constexpr long ShadowAirFade = 120;
}

ska::ShadowSystem::ShadowSystem(ska::EntityManager& entityManager, ska::CameraSystem& camera) :
    AbstractGraphicSystem(camera),
    System(entityManager) {
	m_shadow.load(ska::SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, 13), 2, 1, 1, false, Texture::DEFAULT_T_RED, Texture::DEFAULT_T_GREEN, Texture::DEFAULT_T_BLUE, 127);
}

void ska::ShadowSystem::refresh(unsigned int) {
	assert(m_drawables != nullptr && "Bad initialization");

	const auto& camera = m_camera.getDisplay();

	m_pgd.clear();
	
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto pos = m_componentAccessor.get<PositionComponent>(entityId);
		// Drawn under the sprite (between the logic positions as the sprite)
		auto* graphic = m_componentPossibleAccessor.get<GraphicComponent>(entityId);
		// The shadow of an invisible sprite (ex : a Pokemon recalled in its pokeball) is not drawn, it fades with it
		if (graphic != nullptr && graphic->effect.alpha == 0) {
			continue;
		}
		if (graphic != nullptr) {
			const auto drawn = graphic->drawnPosition(pos);
			pos.x = static_cast<long>(std::lround(drawn.x));
			pos.y = static_cast<long>(std::lround(drawn.y));
		}
		auto& hc = m_componentAccessor.get<HitboxComponent>(entityId);

		// Under its feet : the bottom of its hitbox (its ground line, as GraphicSystem sorts it ; without a hitbox, row 64
		// of a 72 px frame, Sprites/Charsets). The shadow stays on the ground when the character jumps or flies.
		auto spriteSize = Point<int>{ 72, 72 };
		if (graphic != nullptr && !graphic->animatedSprites.empty()) {
			spriteSize = Point<int>{ static_cast<int>(graphic->animatedSprites[0].getWidth()), static_cast<int>(graphic->animatedSprites[0].getHeight()) };
		}
		const auto origin = graphic != nullptr ? graphic->spriteOrigin(pos.x, pos.y, hc) : GraphicComponent{}.spriteOrigin(pos.x, pos.y, hc);
		const auto spriteX = origin.x;
		const auto spriteY = origin.y;
		const auto feetY = hc.height > 0 ? static_cast<int>(pos.y) + hc.yOffset + static_cast<int>(hc.height) - 6 : spriteY + spriteSize.y * 8 / 9 - 6;
		const auto relPosX = spriteX + spriteSize.x / 2 - static_cast<int>(m_shadow.getWidth()) / 2 - camera.x;
		const auto relPosY = feetY - static_cast<int>(m_shadow.getHeight()) / 2 - camera.y;

		const auto priority = static_cast<int>(10);
		if (!((((static_cast<int>(relPosX + m_shadow.getWidth())) < 0) || (relPosX >= camera.w)) ||
			((static_cast<int>(relPosY + m_shadow.getHeight()) < 0) || (relPosY >= camera.h)))) {
			m_pgd.emplace_back(m_shadow, relPosX, relPosY, priority, priority);
			// Its feet hidden (cropped : ex in tall grass), high in the air (ex : a bird flying in, drawn out of the screen) :
			// its shadow fades out
			const auto feetHidden = graphic != nullptr ? std::min(1.F, static_cast<float>(graphic->cropBottom) / static_cast<float>(ShadowCropFade)) : 0.F;
			const auto high = std::clamp(static_cast<float>(pos.z - ShadowAirStart) / static_cast<float>(ShadowAirFade - ShadowAirStart), 0.F, 1.F);
			const auto alpha = static_cast<float>(graphic != nullptr ? graphic->effect.alpha : 255) * (1.F - feetHidden) * (1.F - high);
			if (alpha < 255.F) {
				auto effect = RenderEffect{};
				effect.alpha = static_cast<unsigned char>(alpha);
				m_pgd.back().setEffect(effect);
			}
		}
	}

	for (auto& pgd : m_pgd) {
		m_drawables->add(pgd);
	}

}

void ska::ShadowSystem::update(unsigned int ellapsedTime) {
	System::update(ellapsedTime);
}

