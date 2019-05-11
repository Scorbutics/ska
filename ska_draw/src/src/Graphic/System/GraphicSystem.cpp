#include <limits>
#include "CameraSystem.h"
#include "Core/Draw/DrawableContainer.h"
#include "GraphicSystem.h"
#include "../PositionnedGraphicDrawable.h"
#include "Core/ECS/Basics/Graphic/DialogComponent.h"
#include "Base/Values/Numbers/NumberUtils.h"

ska::GraphicSystem::GraphicSystem(ska::EntityManager& entityManager, ska::GameEventDispatcher& ged, ska::CameraSystem& camera) :
	AbstractGraphicSystem(camera),
	System(entityManager),
	m_ged(ged),
	m_topLayerPriority(0) {
	m_drawables = nullptr;
}

void ska::GraphicSystem::refresh(unsigned int) {
	assert(m_drawables != nullptr && "Bad state");

	const auto camera = m_camera.getDisplay();

	m_topLayerPriority = std::numeric_limits<int>::min();
	m_pgd.clear();
	
	const auto& processed = getEntities();
	for (const auto& entityId : processed) {
		auto& gc = m_componentAccessor.get<ska::GraphicComponent>(entityId);
		auto& hc = m_componentAccessor.get<ska::HitboxComponent>(entityId);
		auto& pos = m_componentAccessor.get<ska::PositionComponent>(entityId);

		const auto& absPos = ska::Point<int> { pos.x - static_cast<int>(hc.width / 2) + hc.xOffset / 2, pos.y - static_cast<int>(hc.height / 2) - 5 };
		const auto& relPosX = static_cast<int>(absPos.x - camera.x);
		const auto& relPosY = static_cast<int>(absPos.y - camera.y - pos.z);

		for (auto& sprite : gc.sprites) {
			m_topLayerPriority = NumberUtils::maximum<int>(static_cast<int>(absPos.y), m_topLayerPriority);
			constexpr auto minInt = std::numeric_limits<int>::min(); 
			const auto priority = gc.desiredPriority > minInt ? gc.desiredPriority : static_cast<int>(absPos.y + camera.h * pos.z);
			m_pgd.emplace_back(sprite, relPosX, relPosY, priority, m_topLayerPriority);
			if(!NumberUtils::isLowValue(pos.rotationY, 0.01) || !NumberUtils::isLowValue(pos.rotationX, 0.01)) {
				m_pgd.back().rotateOnItself(NumberUtils::fastAtan2(pos.rotationY, pos.rotationX));
			}
		}

		for (auto& sprite : gc.animatedSprites) {
			sprite.update();
			m_topLayerPriority = NumberUtils::maximum<int>(static_cast<int>(absPos.y), m_topLayerPriority);
			constexpr auto minInt = std::numeric_limits<int>::min();
			const auto priority = gc.desiredPriority > minInt ? gc.desiredPriority : static_cast<int>(absPos.y + camera.h * pos.z);
			const PositionnedGraphicDrawable pgd( sprite, relPosX, relPosY, priority, m_topLayerPriority );
			m_pgd.push_back(pgd);
			if(!NumberUtils::isLowValue(pos.rotationY, 0.01) || !NumberUtils::isLowValue(pos.rotationX, 0.01)) {
				m_pgd.back().rotateOnItself(NumberUtils::fastAtan2(pos.rotationY, pos.rotationX));
			}
		}

		const auto& dcPtr = m_componentPossibleAccessor.get<DialogComponent>(entityId);
		if (dcPtr != nullptr) {
			auto& dc = *dcPtr;
			GUIEvent ge(GUIEventType::REFRESH_BALLOON);
			ge.balloonHandle = dc.handle;
			ge.windowName = dc.name;
			ge.balloonPosition = { static_cast<int>(absPos.x - camera.x), static_cast<int>(absPos.y - camera.y) };
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
