#pragma once
#include "DirectionnalAnimationStateMachine.h"
#include "JumpAnimationStateMachine.h"

namespace ska {
    class EntityManager;

    class WalkAnimationStateMachine :
		public AnimationStateMachine<JumpAnimationStateMachine> {
	public:
		explicit WalkAnimationStateMachine(EntityManager& em);
	    virtual ~WalkAnimationStateMachine() = default;

		virtual void update(AnimationComponent&, EntityId&) override;
		virtual void onEnter(EntityId&) override;
    private:
		ska::EntityManager& m_entityManager;
	};
}

