#pragma once
#include "CameraStrategy.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Graphic/CameraFocusedComponent.h"

namespace ska {
	class CameraFollowStrategy : 
		public CameraStrategy {
	public:
		CameraFollowStrategy(ska::EntityManager& em);
		~CameraFollowStrategy() override = default;
		
		std::pair<Rectangle, EntityId*> focus(const std::unordered_set<EntityId>& processed) override;
	
	private:
		EntityManager& m_entityManager;
	};
}