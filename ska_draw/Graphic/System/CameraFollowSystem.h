#pragma once
#include "CameraSystem.h"
#include "Rectangle.h"

namespace ska {
	class CameraFollowSystem : public CameraSystem {
	public:
		CameraFollowSystem(EntityManager& entityManager, GameEventDispatcher& ged, const unsigned int screenW = 0, const unsigned int screenH = 0);
		~CameraFollowSystem();

		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		Rectangle m_follow;
	};
}
