#pragma once
#include "AnimationStateMachine.h"

namespace ska {
    class EntityManager;

    class WalkAnimationStateMachine :
        public AnimationStateMachine {
	public:
		WalkAnimationStateMachine(EntityManager& em);
		virtual ~WalkAnimationStateMachine() = default;

		virtual void animate(EntityId& entityId, AnimationComponent& ac) override;

    private:
        EntityManager& m_entityManager;
	};
}

