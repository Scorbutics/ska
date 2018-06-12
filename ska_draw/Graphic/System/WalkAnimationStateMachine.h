#pragma once
#include <unordered_map>
#include "JumpAnimationStateMachine.h"
#include "ECS/Basics/Physic/PositionComponent.h"

namespace ska {
    class EntityManager;

    class WalkAnimationStateMachine :
		public AnimationStateMachine<JumpAnimationStateMachine> {
	public:
		explicit WalkAnimationStateMachine(EntityManager& em);
	    virtual ~WalkAnimationStateMachine() = default;

		virtual void update(AnimationComponent&, const EntityId&) override;
		virtual void onEnter(const EntityId&) override;
    private:
		ska::EntityManager& m_entityManager;
		std::unordered_map<ska::EntityId, ska::PositionComponent> m_lastPos;
	};
}

