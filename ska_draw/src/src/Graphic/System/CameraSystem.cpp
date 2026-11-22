#include "Graphic/__internalConfig/LoggerConfig.h"
#include "CameraSystem.h"
#include <cmath>
#include <algorithm>
#include "Core/Rectangle.h"
#include "../GraphicComponent.h"

ska::CameraSystem::CameraSystem(EntityManager& entityManager, GameEventDispatcher& ged, CameraStrategyPtr strategy, const unsigned int screenW, const unsigned int screenH) :
	System(entityManager),
	SubObserver<GameEvent>(std::bind(&CameraSystem::onGameEvent, this, std::placeholders::_1), ged),
	SubObserver<WorldEvent>(std::bind(&CameraSystem::onWorldEvent, this, std::placeholders::_1), ged),
	m_strategy(std::move(strategy)) {

	assert(m_strategy != nullptr && "Cannot use a null camera strategy : pick one");
	
	worldResized(screenW, screenH);
	m_screenWidth = static_cast<int>(screenW);
	m_screenHeight = static_cast<int>(screenH);
	resizeDisplay();
}

void ska::CameraSystem::worldResized(const unsigned int worldW, const unsigned int worldH) {
	m_worldW = worldW;
	m_worldH = worldH;
}

ska::Point<int> ska::CameraSystem::getScreenSize() const {
	return ska::Point<int>(m_screenWidth, m_screenHeight);
}

void ska::CameraSystem::resizeDisplay() {
	m_cameraRect.w = static_cast<int>(std::ceil(static_cast<float>(m_screenWidth) / m_zoom));
	m_cameraRect.h = static_cast<int>(std::ceil(static_cast<float>(m_screenHeight) / m_zoom));
}

void ska::CameraSystem::zoomTo(float zoom, float speed) {
	m_targetZoom = std::max(0.25F, zoom);
	m_zoomSpeed = speed;
}

void ska::CameraSystem::setZoom(float zoom) {
	m_targetZoom = m_zoom = std::max(0.25F, zoom);
	resizeDisplay();
}

bool ska::CameraSystem::onGameEvent(ska::GameEvent& ge) {
	m_screenWidth = static_cast<int>(ge.windowWidth);
	m_screenHeight = static_cast<int>(ge.windowHeight);
	resizeDisplay();
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

void ska::CameraSystem::focusOn(Rectangle& pos, const EntityId* entityId) {
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
	// A jump of the camera (another map, a teleport) is not smoothed
	constexpr auto snapDistance = 96.F;
	const auto display = m_displayInterpolation.sample({ static_cast<float>(m_cameraRect.x), static_cast<float>(m_cameraRect.y), 0.F }, snapDistance);
	m_displayRect = m_cameraRect;
	m_displayRect.x = static_cast<int>(std::lround(display.x));
	m_displayRect.y = static_cast<int>(std::lround(display.y));
	return m_displayRect;
}

void ska::CameraSystem::shake(unsigned int duration, int intensity) {
	m_shakeDuration = duration;
	m_shakeElapsed = 0;
	m_shakeIntensity = intensity < 0 ? -intensity : intensity;
}

void ska::CameraSystem::refresh(unsigned int ellapsedTime) {
	// The zoom goes to its target smoothly
	if (m_zoom != m_targetZoom) {
		const auto step = 1.F - std::exp(-m_zoomSpeed * static_cast<float>(ellapsedTime) / 1000.F);
		m_zoom += (m_targetZoom - m_zoom) * step;
		if (std::abs(m_targetZoom - m_zoom) < 0.002F) {
			m_zoom = m_targetZoom;
		}
		resizeDisplay();
	}
	auto[pos, entityId] = m_strategy->focus(getEntities());
	focusOn(pos, entityId);

	if (m_shakeElapsed < m_shakeDuration && m_shakeIntensity > 0) {
		m_shakeElapsed += ellapsedTime;
		// Less and less strong, until the end of the duration
		const auto remaining = m_shakeElapsed >= m_shakeDuration ? 0.F : 1.F - static_cast<float>(m_shakeElapsed) / static_cast<float>(m_shakeDuration);
		const auto amplitude = static_cast<int>(static_cast<float>(m_shakeIntensity) * remaining);
		if (amplitude > 0) {
			auto offset = std::uniform_int_distribution<int>{ -amplitude, amplitude };
			m_cameraRect.x += offset(m_shakeRandom);
			m_cameraRect.y += offset(m_shakeRandom);
		}
	}
}
