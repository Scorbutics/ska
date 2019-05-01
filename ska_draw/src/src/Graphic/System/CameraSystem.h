#pragma once
#include <unordered_set>
#include "Rectangle.h"
#include "ECS/System.h"
#include "Data/Events/WorldEvent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Graphic/CameraFocusedComponent.h"
#include "../GraphicComponent.h"
#include "Utils/SubObserver.h"
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

		~CameraSystem() override = default;

	private:
		void focusOn(Rectangle& pos, EntityId* optionalEntityId);

		unsigned int m_worldW, m_worldH;
		Rectangle m_cameraRect{};

		CameraStrategyPtr m_strategy;

	    void worldResized(const unsigned int worldW, const unsigned int worldH);

	    bool onWorldEvent(ska::WorldEvent& we);
		bool onGameEvent(ska::GameEvent& ge);
	};
}
