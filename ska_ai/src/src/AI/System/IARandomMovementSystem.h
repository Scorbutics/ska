#pragma once
#include <unordered_set>
#include "../IARandomMovementComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/PositionComponent.h"


namespace ska {
	class IARandomMovementSystem : public System< RequiredComponent<IARandomMovementComponent, ForceComponent>, PossibleComponent<PositionComponent, AnimationComponent>> {
	public:
		explicit IARandomMovementSystem(EntityManager& entityManager);
		IARandomMovementSystem& operator=(const IARandomMovementSystem&) = delete;
		virtual ~IARandomMovementSystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;

	};
}

