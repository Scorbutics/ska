#pragma once
#include "AnimationStateMachine.h"
#include "ECS/Basics/Graphic/AnimationStateMachine.h"

namespace ska {
    class EntityManager;

    class WalkAnimationStateMachine :
        public AnimationStateMachine {
	public:
		explicit WalkAnimationStateMachine(EntityManager& em);
	    virtual ~WalkAnimationStateMachine() = default;

		virtual void onEnter() override;
		virtual AnimationStateMachine* animate(AnimationComponent& ac, EntityId& entityId) override;

    private:
        EntityManager& m_entityManager;
	};
}

