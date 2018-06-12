#pragma once
#include "AnimationStateMachine.h"

namespace ska {
	class EntityManager;
	class WalkAnimationStateMachine;

	class JumpAnimationStateMachine :
		public AnimationStateMachine<WalkAnimationStateMachine> {
	public:
		explicit JumpAnimationStateMachine(ska::EntityManager& em);
		virtual void update(ska::AnimationComponent& ac, const EntityId& entityId) override;
		virtual void onEnter(const EntityId&) override;
	private:
		ska::EntityManager& m_entityManager;
	};
}

