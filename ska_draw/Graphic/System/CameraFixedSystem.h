#pragma once
#include "CameraSystem.h"
#include "Rectangle.h"

namespace ska {

	class CameraFixedSystem : public CameraSystem {
	public:
		CameraFixedSystem(EntityManager& entityManager, GameEventDispatcher& ged, const unsigned int screenW = 0, const unsigned int screenH = 0, Point<int> point = ska::Point<int>());
		CameraFixedSystem& operator=(const CameraFixedSystem&) = delete;
		~CameraFixedSystem();

		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		Rectangle m_fixed;

	};
}
