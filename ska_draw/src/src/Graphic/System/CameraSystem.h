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

		Point<int> getScreenSize() const;
		void changeStrategy(CameraStrategyPtr strategy);
		const Rectangle& getDisplay() const;
		// Shakes the camera during "duration" milliseconds : the display moves randomly up to "intensity" pixels, less and less
		void shake(unsigned int duration, int intensity);

		~CameraSystem() override = default;

	private:
		void focusOn(Rectangle& pos, EntityId* optionalEntityId);

		unsigned int m_worldW, m_worldH;
		Rectangle m_cameraRect{};

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
