#pragma once
#include <unordered_set>
#include "../IADefinedMovementComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"


namespace ska { 
	class IADefinedMovementSystem : public System< RequiredComponent<IADefinedMovementComponent, MovementComponent, PositionComponent, HitboxComponent, CollidableComponent>, PossibleComponent<WorldCollisionComponent>> {
	public:
		explicit IADefinedMovementSystem(EntityManager& entityManager, GameEventDispatcher& ged);
		virtual ~IADefinedMovementSystem();
	protected:
		void refresh(unsigned int ellapsedTime) override;
	private:
		GameEventDispatcher& m_eventDispatcher;
	};
}

