#pragma once
#include "../IAFollowComponent.h"
#include "../IADefinedMovementComponent.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"

namespace ska {
	// An entity following another one walks towards it when it is too far (more than its follow distance, plus a margin) :
	// it gets a defined movement (IADefinedMovementComponent) to the follow distance of its target.
	// The following stops when the target does not exist anymore.
	class IAFollowSystem : public System<RequiredComponent<IAFollowComponent, PositionComponent, HitboxComponent>, PossibleComponent<IADefinedMovementComponent>> {
	public:
		explicit IAFollowSystem(EntityManager& entityManager);
		IAFollowSystem& operator=(const IAFollowSystem&) = delete;
		~IAFollowSystem() override = default;

		// The follower starts walking when it is farther than its follow distance plus this margin (pixels)
		static constexpr float Margin = 24.F;

	protected:
		void refresh(unsigned int ellapsedTime) override;

	private:
		EntityManager& m_entities;
	};
}
