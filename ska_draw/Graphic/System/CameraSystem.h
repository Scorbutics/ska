#pragma once
#include <unordered_set>
#include "Rectangle.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Graphic/CameraFocusedComponent.h"
#include "../GraphicComponent.h"
#include "Utils/SubObserver.h"

namespace ska {
	template <class T>
	struct Point;

	class CameraSystem : 
		public System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, CameraFocusedComponent>, PossibleComponent<HitboxComponent>>,
		public SubObserver<GameEvent> {
	public:
		CameraSystem(EntityManager& entityManager, GameEventDispatcher& ged, const unsigned int screenW = 0, const unsigned int screenH = 0);
		CameraSystem& operator=(const CameraSystem&) = delete;
		void worldResized(const unsigned int worldW, const unsigned int worldH);

		Point<int> getScreenSize() const;
		

		virtual const Rectangle* getDisplay() const;
		virtual ~CameraSystem();

	protected:
		void focusOn(Rectangle& pos, EntityId* optionalEntityId);

		unsigned int m_worldW, m_worldH;
		Rectangle* m_pos;
		Rectangle m_cameraRect;

	private:
		bool onGameEvent(ska::GameEvent& ge);
	};
}
