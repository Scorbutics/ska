#pragma once
#include <random>
#include <unordered_set>
#include "Core/Rectangle.h"
#include "Core/ECS/System.h"
#include "Core/Data/Events/WorldEvent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Graphic/CameraFocusedComponent.h"
#include "../GraphicComponent.h"
#include "Base/Patterns/SubObserver.h"
#include "CameraStrategy.h"
#include "Core/Draw/FrameClock.h"

namespace ska {
	template <class T>
	struct Point;

	class CameraSystem :
		public System< RequiredComponent<PositionComponent, CameraFocusedComponent>, PossibleComponent<HitboxComponent>>,
		public SubObserver<GameEvent>,
		public SubObserver<WorldEvent> {

	public:
		CameraSystem(EntityManager& entityManager, GameEventDispatcher& ged, CameraStrategyPtr strategy, const unsigned int screenW = 0, const unsigned int screenH = 0);
		CameraSystem& operator=(const CameraSystem&) = delete;

		void refresh(unsigned int ellapsedTime) override;

		// The size of the screen (pixels) ; the display (getDisplay) is the world seen : this size divided by the zoom
		Point<int> getScreenSize() const;
		// Zoom of the world (1 : none, 2 : twice bigger). zoomTo goes to it smoothly ("speed" : the part of the way done
		// each second, 3 : quick), setZoom at once
		void zoomTo(float zoom, float speed = 3.F);
		void setZoom(float zoom);
		float getZoom() const { return m_zoom; }
		void changeStrategy(CameraStrategyPtr strategy);
		// The display to draw : between its last two logic positions (FrameClock), as the drawn entities
		const Rectangle& getDisplay() const;
		// Shakes the camera during "duration" milliseconds : the display moves randomly up to "intensity" pixels, less and less
		void shake(unsigned int duration, int intensity);

		~CameraSystem() override = default;

	private:
		void focusOn(Rectangle& pos, const EntityId* optionalEntityId);

		unsigned int m_worldW, m_worldH;
		Rectangle m_cameraRect{};
		int m_screenWidth = 0;
		int m_screenHeight = 0;
		float m_zoom = 1.F;
		float m_targetZoom = 1.F;
		float m_zoomSpeed = 3.F;
		void resizeDisplay();
		mutable Rectangle m_displayRect{};
		mutable Interpolated m_displayInterpolation;

		CameraStrategyPtr m_strategy;

	    void worldResized(const unsigned int worldW, const unsigned int worldH);

	    bool onWorldEvent(ska::WorldEvent& we);
		bool onGameEvent(ska::GameEvent& ge);
		unsigned int m_shakeDuration = 0;
		unsigned int m_shakeElapsed = 0;
		int m_shakeIntensity = 0;
		std::minstd_rand m_shakeRandom;
	};
}
