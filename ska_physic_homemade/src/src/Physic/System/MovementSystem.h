#pragma once
#include <unordered_set>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/Ticked.h"

namespace ska {
	class MovementSystem : public System< ska::RequiredComponent<PositionComponent, MovementComponent, ForceComponent>, ska::PossibleComponent<>> {
	public:
		MovementSystem(EntityManager& entityManager);
		MovementSystem& operator=(const MovementSystem&) = delete;
		virtual ~MovementSystem();
	
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	
	};
}
