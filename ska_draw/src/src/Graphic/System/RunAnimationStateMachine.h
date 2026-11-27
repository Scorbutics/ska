#pragma once
#include "AnimationStateMachine.h"

namespace ska {
	class EntityManager;
	class WalkAnimationStateMachine;
	class JumpAnimationStateMachine;
	class PoseAnimationStateMachine;

	// The run : as the walk driven by a movement system (AnimationComponent::walking, walkDelay, state), from another block
	// of columns of the character sheet (firstColumn : its 3 frames, ex the columns 3 to 5 after the walk)
	class RunAnimationStateMachine :
		public AnimationStateMachine<WalkAnimationStateMachine, JumpAnimationStateMachine, PoseAnimationStateMachine> {
	public:
		RunAnimationStateMachine(EntityManager& em, unsigned int firstColumn);
		~RunAnimationStateMachine() override = default;

		void update(AnimationComponent& ac, const EntityId& entityId) override;
		void onEnter(const EntityId& entityId) override;
	private:
		EntityManager& m_entityManager;
		unsigned int m_firstColumn;
	};
}
