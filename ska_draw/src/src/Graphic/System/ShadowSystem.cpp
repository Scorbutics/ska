#include "Graphic/__internalConfig/LoggerConfig.h"
#include "ShadowSystem.h"
#include <cmath>
#include "Core/Utils/SpritePath.h"
#include "CameraSystem.h"
#include "Core/Draw/DrawableContainer.h"
#include "../PositionnedGraphicDrawable.h"
#include "Core/Utils/SkaConstants.h"

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
		if (graphic != nullptr) {
			const auto drawn = graphic->drawnPosition(pos);
			pos.x = static_cast<long>(std::lround(drawn.x));
			pos.y = static_cast<long>(std::lround(drawn.y));
		}
		auto& hc = m_componentAccessor.get<HitboxComponent>(entityId);

		// Under the feet of the sprite : the sprite is drawn as GraphicSystem does, the feet of the characters are on the
		// bottom row of their block (row 64 of a 72 px frame, Sprites/Charsets). The shadow stays on the ground when the
		// character jumps.
		auto spriteSize = Point<int>{ 72, 72 };
		if (graphic != nullptr && !graphic->animatedSprites.empty()) {
			spriteSize = Point<int>{ static_cast<int>(graphic->animatedSprites[0].getWidth()), static_cast<int>(graphic->animatedSprites[0].getHeight()) };
		}
		const auto spriteX = static_cast<int>(pos.x) - static_cast<int>(hc.width / 2) + hc.xOffset / 2;
		const auto spriteY = static_cast<int>(pos.y) - static_cast<int>(hc.height / 2) - 5;
		const auto feetY = spriteY + spriteSize.y * 8 / 9 - 6;
		const auto relPosX = spriteX + spriteSize.x / 2 - static_cast<int>(m_shadow.getWidth()) / 2 - camera.x;
		const auto relPosY = feetY - static_cast<int>(m_shadow.getHeight()) / 2 - camera.y;

		const auto priority = static_cast<int>(10);
		if (!((((static_cast<int>(relPosX + m_shadow.getWidth())) < 0) || (relPosX >= camera.w)) ||
			((static_cast<int>(relPosY + m_shadow.getHeight()) < 0) || (relPosY >= camera.h)))) {
			m_pgd.emplace_back(m_shadow, relPosX, relPosY, priority, priority);
		}
	}

	for (auto& pgd : m_pgd) {
		m_drawables->add(pgd);
	}

}

void ska::ShadowSystem::update(unsigned int ellapsedTime) {
	System::update(ellapsedTime);
}

