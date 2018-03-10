#include <limits>
#include "CameraSystem.h"
#include "Draw/DrawableContainer.h"
#include "GraphicSystem.h"
#include "../PositionnedGraphicDrawable.h"
#include "ECS/Basics/Graphic/DialogComponent.h"
#include "Utils/NumberUtils.h"

ska::GraphicSystem::GraphicSystem(ska::EntityManager& entityManager, ska::GameEventDispatcher& ged, ska::CameraSystem* camera) :
	AbstractGraphicSystem(camera),
	System(entityManager),
	m_ged(ged),
	m_topLayerPriority(0) {
	m_drawables = nullptr;
}

void ska::GraphicSystem::refresh(unsigned int) {
	const auto camera = m_camera == nullptr ? nullptr : m_camera->getDisplay();
	const unsigned int cameraX = (camera == nullptr || camera->x < 0 ? 0 : camera->x);
	const unsigned int cameraY = (camera == nullptr || camera->y < 0 ? 0 : camera->y);

	if (m_drawables == nullptr) {
		return;
	}

	m_topLayerPriority = std::numeric_limits<int>::min();
	m_pgd.clear();
	
	const auto& processed = getEntities();
	for (const auto& entityId : processed) {
		auto& gc = m_componentAccessor.get<ska::GraphicComponent>(entityId);
		auto& pos = m_componentAccessor.get<ska::PositionComponent>(entityId);
		const auto& relPosX = static_cast<int>(pos.x - cameraX);
		const auto& relPosY = static_cast<int>(pos.y - cameraY - pos.z);

		for (auto& sprite : gc.sprites) {
			m_topLayerPriority = NumberUtils::maximum<int>(static_cast<int>(pos.y), m_topLayerPriority);
			constexpr auto minInt = std::numeric_limits<int>::min(); 
			const auto priority = gc.desiredPriority > minInt ? gc.desiredPriority : static_cast<int>(pos.y + (camera == nullptr ? 0 : camera->h * pos.z));
			m_pgd.emplace_back(sprite, relPosX, relPosY, priority, m_topLayerPriority);
		}

		for (auto& sprite : gc.animatedSprites) {
			sprite.update();
			m_topLayerPriority = NumberUtils::maximum<int>(static_cast<int>(pos.y), m_topLayerPriority);
			constexpr auto minInt = std::numeric_limits<int>::min();
			const auto priority = gc.desiredPriority > minInt ? gc.desiredPriority : static_cast<int>(pos.y + (camera == nullptr ? 0 : camera->h * pos.z));
			const PositionnedGraphicDrawable pgd( sprite, relPosX, relPosY, priority, m_topLayerPriority );
			m_pgd.push_back(pgd);
		}

		const auto& dcPtr = m_componentPossibleAccessor.get<DialogComponent>(entityId);
		if (dcPtr != nullptr) {
			auto& dc = *dcPtr;
			GUIEvent ge(GUIEventType::REFRESH_BALLOON);
			ge.balloonHandle = dc.handle;
			ge.windowName = dc.name;
			ge.balloonPosition = { static_cast<int>(pos.x - cameraX), static_cast<int>(pos.y - cameraY) };
			m_ged.ska::Observable<GUIEvent>::notifyObservers(ge);

			if(ge.balloonHandle == nullptr) {
				m_componentAccessor.remove<DialogComponent>(entityId);
			}
		}
	}
	for (auto& pgd : m_pgd) {
		m_drawables->add(pgd);
	}

	m_drawables = nullptr;
}

void ska::GraphicSystem::update(unsigned int ellapsedTime) {
	System::update(ellapsedTime);
}

int ska::GraphicSystem::getTopLayerPriority() const {
	return m_topLayerPriority + 1;
}

ska::GraphicSystem::~GraphicSystem() {
}
