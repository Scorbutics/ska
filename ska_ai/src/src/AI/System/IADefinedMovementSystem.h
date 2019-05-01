#pragma once
#include <unordered_set>
#include "../IADefinedMovementComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"

namespace ska { 
	namespace cp {
		class Space;
	}

	class IADefinedMovementSystem : public System< RequiredComponent<IADefinedMovementComponent, MovementComponent, PositionComponent, HitboxComponent, cp::HitboxComponent, CollidableComponent, ForceComponent>, PossibleComponent<WorldCollisionComponent>> {
	public:
		explicit IADefinedMovementSystem(cp::Space& space, EntityManager& entityManager, GameEventDispatcher& ged);
		virtual ~IADefinedMovementSystem();
	protected:
		void refresh(unsigned int ellapsedTime) override;
	
	private:
		cp::Space& m_space;
		GameEventDispatcher& m_eventDispatcher;
	};
}

