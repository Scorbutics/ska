#pragma once
#include <unordered_set>
#include "../IADefinedMovementComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/WorldCollisionComponent.h"

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

