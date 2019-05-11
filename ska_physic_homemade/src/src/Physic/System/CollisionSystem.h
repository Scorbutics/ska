#pragma once
#include <unordered_set>
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/Data/Events/CollisionEvent.h"
#include "Core/Rectangle.h"
#include "Core/ECS/System.h"
#include "Core/Data/Events/GameEventDispatcher.h"

namespace ska {

	class CollisionSystem : public System< RequiredComponent<PositionComponent, MovementComponent, HitboxComponent, CollidableComponent>, PossibleComponent<>> {
	public:
		CollisionSystem(EntityManager& entityManager, GameEventDispatcher& ged);
		CollisionSystem& operator=(const CollisionSystem&) = delete;
		virtual ~CollisionSystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		inline Rectangle createHitBox(EntityId entityId) const;
		GameEventDispatcher& m_ged;
	};
}
