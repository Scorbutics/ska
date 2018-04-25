#pragma once
#include <unordered_set>
#include "Rectangle.h"
#include "ECS/System.h"
#include "Data/Events/WorldEvent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Graphic/CameraFocusedComponent.h"
#include "../GraphicComponent.h"
#include "Utils/SubObserver.h"

namespace ska {
	template <class T>
	struct Point;

	class CameraSystem :
		public System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, CameraFocusedComponent>, PossibleComponent<HitboxComponent>>,
		public SubObserver<GameEvent>,
		public SubObserver<WorldEvent> {

	public:
		CameraSystem(EntityManager& entityManager, GameEventDispatcher& ged, const unsigned int screenW = 0, const unsigned int screenH = 0);
		CameraSystem& operator=(const CameraSystem&) = delete;

		Point<int> getScreenSize() const;

		virtual const Rectangle* getDisplay() const;
		virtual ~CameraSystem();

	protected:
		void focusOn(Rectangle& pos, EntityId* optionalEntityId);

		unsigned int m_worldW, m_worldH;
		Rectangle* m_pos;
		Rectangle m_cameraRect;

	private:
	    void worldResized(const unsigned int worldW, const unsigned int worldH);

	    bool onWorldEvent(ska::WorldEvent& we);
		bool onGameEvent(ska::GameEvent& ge);
	};
}
