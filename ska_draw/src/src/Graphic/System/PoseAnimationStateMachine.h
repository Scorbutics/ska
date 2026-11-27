#pragma once
#include "AnimationStateMachine.h"

namespace ska {
	class EntityManager;
	class WalkAnimationStateMachine;
	class RunAnimationStateMachine;
	class JumpAnimationStateMachine;

	// A single frame of the character sheet (the column AnimationComponent::pose, the row of its direction), not animated :
	// a pose between two animations (ex : setting off, braking), as long as the links of the game keep it
	class PoseAnimationStateMachine :
		public AnimationStateMachine<WalkAnimationStateMachine, RunAnimationStateMachine, JumpAnimationStateMachine> {
	public:
		explicit PoseAnimationStateMachine(EntityManager& em);
		~PoseAnimationStateMachine() override = default;

		void update(AnimationComponent& ac, const EntityId& entityId) override;
		void onEnter(const EntityId& entityId) override;
	private:
		EntityManager& m_entityManager;
	};
}
