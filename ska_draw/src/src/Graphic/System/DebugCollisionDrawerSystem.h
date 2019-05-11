#pragma once
#include <unordered_set>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Core/ECS/Basics/Graphic/DebugGraphicComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"

namespace ska {
	class DebugCollisionDrawerSystem : 
		public System< RequiredComponent<PositionComponent, HitboxComponent, DebugGraphicComponent>, PossibleComponent<>> {
	public:
		DebugCollisionDrawerSystem(EntityManager& em);
		EntityId createDebugCollisionEntity(const Point<int>& p, int offset);
		virtual void refresh(unsigned int ellapsedTime) override;
		virtual ~DebugCollisionDrawerSystem() = default;
	};
}
