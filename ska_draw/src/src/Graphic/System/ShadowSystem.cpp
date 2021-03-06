#include "Graphic/__internalConfig/LoggerConfig.h"
#include "ShadowSystem.h"
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
		auto& pos = m_componentAccessor.get<PositionComponent>(entityId);
		auto& hc = m_componentAccessor.get<HitboxComponent>(entityId);

		const auto relPosX = static_cast<int>(pos.x + 12 - camera.x);
		const auto relPosY = static_cast<int>(pos.y + hc.yOffset / 2 - camera.y - m_shadow.getHeight()/2);

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

