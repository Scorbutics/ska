#include "ShadowSystem.h"
#include "Utils/SpritePath.h"
#include "CameraSystem.h"
#include "Draw/DrawableContainer.h"
#include "../PositionnedGraphicDrawable.h"
#include "Utils/SkaConstants.h"

ska::ShadowSystem::ShadowSystem(ska::EntityManager& entityManager, ska::CameraSystem* camera) :
    AbstractGraphicSystem(camera),
    System(entityManager) {
	m_shadow.load(ska::SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, 13), 2, 1, 1, false, Texture::DEFAULT_T_RED, Texture::DEFAULT_T_GREEN, Texture::DEFAULT_T_BLUE, 127);
}

void ska::ShadowSystem::refresh(unsigned int) {
	const ska::Rectangle* camera = m_camera == nullptr ? nullptr : m_camera->getDisplay();
	const unsigned int cameraX = (camera == nullptr || camera->x < 0 ? 0 : camera->x);
	const unsigned int cameraY = (camera == nullptr || camera->y < 0 ? 0 : camera->y);

	if (m_drawables == nullptr) {
		return;
	}

	m_pgd.clear();
	
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& pos = m_componentAccessor.get<ska::PositionComponent>(entityId);
		auto& hc = m_componentAccessor.get<ska::HitboxComponent>(entityId);

		const auto relPosX = static_cast<int>(pos.x + hc.xOffset - cameraX);
		const auto relPosY = static_cast<int>(pos.y + hc.yOffset - cameraY - m_shadow.getHeight()/3);
		const auto priority = static_cast<int>(pos.y - (camera == nullptr ? 0 : camera->h) - 10);
		if (!((((static_cast<int>(relPosX + m_shadow.getWidth())) < 0) || (camera != nullptr && relPosX >= camera->w)) ||
			((static_cast<int>(relPosY + m_shadow.getHeight()) < 0) || (camera != nullptr && relPosY >= camera->h)))) {
			m_pgd.push_back(PositionnedGraphicDrawable{ m_shadow, relPosX, relPosY, priority, priority });
		}
	}

	for (auto& pgd : m_pgd) {
		m_drawables->add(pgd);
	}

}

void ska::ShadowSystem::update(unsigned int ellapsedTime) {
	System::update(ellapsedTime);
}

ska::ShadowSystem::~ShadowSystem() {
}
