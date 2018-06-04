#pragma once
#include <unordered_set>
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Data/Events/CollisionEvent.h"
#include "Rectangle.h"
#include "ECS/System.h"
#include "Data/Events/GameEventDispatcher.h"

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
