#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "ECS/Basics/Graphic/AnimationComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/System.h"
#include "AnimationStateMachine.h"

namespace ska {
	class AnimationSystem :
	    public System<std::unordered_set<EntityId>, RequiredComponent<GraphicComponent, MovementComponent, AnimationComponent>, PossibleComponent<PositionComponent>> {
	public:
		explicit AnimationSystem(EntityManager& entityManager);
		AnimationSystem& operator=(const AnimationSystem&) = delete;
		virtual ~AnimationSystem() = default;

		virtual void refresh(unsigned int ellapsedTime) override;
        void addAnimationStateMachine(std::unique_ptr<AnimationStateMachine>&& afsm);

    private:
        std::vector<std::unique_ptr<AnimationStateMachine>> m_animationFSM;
	};
}

