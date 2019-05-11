#pragma once
#include <unordered_set>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/GravityAffectedComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"

namespace ska {
	class GravitySystem : public System< RequiredComponent<ForceComponent, GravityAffectedComponent, MovementComponent>, PossibleComponent<>> {
	public:
		explicit GravitySystem(EntityManager& entityManager);
		GravitySystem& operator=(const GravitySystem&) = delete;
		virtual ~GravitySystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;

	};
}
