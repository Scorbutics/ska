#include "CameraSystem.h"
#include "Rectangle.h"
#include "../GraphicComponent.h"

ska::CameraSystem::CameraSystem(EntityManager& entityManager, GameEventDispatcher& ged, CameraStrategyPtr strategy, const unsigned int screenW, const unsigned int screenH) :
	System(entityManager),
	SubObserver<GameEvent>(std::bind(&CameraSystem::onGameEvent, this, std::placeholders::_1), ged),
	SubObserver<WorldEvent>(std::bind(&CameraSystem::onWorldEvent, this, std::placeholders::_1), ged),
	m_strategy(std::move(strategy)) {

	assert(m_strategy != nullptr && "Cannot use a null camera strategy : pick one");
	
	worldResized(screenW, screenH);
	m_cameraRect.w = screenW;
	m_cameraRect.h = screenH;
}

void ska::CameraSystem::worldResized(const unsigned int worldW, const unsigned int worldH) {
	m_worldW = worldW;
	m_worldH = worldH;
}

ska::Point<int> ska::CameraSystem::getScreenSize() const {
	return ska::Point<int>(m_cameraRect.w, m_cameraRect.h);
}

bool ska::CameraSystem::onGameEvent(ska::GameEvent& ge) {
	m_cameraRect.w = ge.windowWidth;
	m_cameraRect.h = ge.windowHeight;
	return true;
}

bool ska::CameraSystem::onWorldEvent(ska::WorldEvent& we) {
	worldResized(we.blocksWidth * we.blockSize, we.blocksHeight * we.blockSize);
	return true;
}

void ska::CameraSystem::changeStrategy(ska::CameraStrategyPtr strategy) {
	m_strategy = std::move(strategy);
	assert(m_strategy != nullptr && "Cannot use a null camera strategy : pick one");
}

void ska::CameraSystem::focusOn(Rectangle& pos, EntityId* entityId) {
	m_cameraRect.x = pos.x - m_cameraRect.w / 2;
	m_cameraRect.y = pos.y - m_cameraRect.h / 2;

	const auto& hitboxComponentPtr = entityId == nullptr ? nullptr : m_componentPossibleAccessor.get<HitboxComponent>(*entityId);
	if (entityId != nullptr && hitboxComponentPtr != nullptr) {
		auto& hc = *hitboxComponentPtr;
		m_cameraRect.x += (hc.width + hc.xOffset) / 2;
		m_cameraRect.y += (hc.height + hc.yOffset) / 2;
	}

	if (m_cameraRect.x < 0) {
		m_cameraRect.x = 0;
	} else if (static_cast<unsigned int>(m_cameraRect.x + m_cameraRect.w) > m_worldW) {
		m_cameraRect.x = m_worldW - m_cameraRect.w;
	}

	if (m_cameraRect.y < 0) {
		m_cameraRect.y = 0;
	} else if (static_cast<unsigned int>(m_cameraRect.y + m_cameraRect.h) > m_worldH) {
		m_cameraRect.y = m_worldH - m_cameraRect.h;
	}
}


const ska::Rectangle& ska::CameraSystem::getDisplay() const {
	return m_cameraRect;
}

void ska::CameraSystem::refresh(unsigned int ellapsedTime) {
	auto[pos, entityId] = m_strategy->focus(getEntities());
	focusOn(pos, entityId);
}
