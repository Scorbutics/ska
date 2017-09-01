/*
#include "AnimationSystem.h"
#include "ECS/Basics/Graphic/AnimationComponent.h"

ska::AnimationSystem::AnimationSystem(EntityManager& entityManager) :
    System(entityManager) {
}

void ska::AnimationSystem::refresh(unsigned int) {
	if(m_animationFSM.empty()) {
        return;
	}

	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& ac = m_componentAccessor.get<AnimationComponent>(entityId);
		if(ac.indexAnimationFiniteStateMachine >= static_cast<int>(m_animationFSM.size())) {
            throw ska::IllegalStateException("Dépassement d'index dans les machines d'état d'animation");
		}
		m_animationFSM[ac.indexAnimationFiniteStateMachine]->animate(entityId, ac);
	}
}

void ska::AnimationSystem::addAnimationStateMachine(std::unique_ptr<AnimationStateMachine>&& afsm) {
    m_animationFSM.push_back(std::forward<std::unique_ptr<AnimationStateMachine>>(afsm));
}
*/
