
#include "AnimationComponent.h"
#include "IAnimationStateMachine.h"

ska::AnimationComponent::AnimationComponent() :
	state(0),
    animationFiniteStateMachine(nullptr) {
}

ska::AnimationComponent::~AnimationComponent(){}

void ska::AnimationComponent::setASM(IAnimationStateMachine& afsm, const EntityId& entityId){
	animationFiniteStateMachine = &afsm;
	animationFiniteStateMachine->onEnter(entityId);
}

ska::IAnimationStateMachine* ska::AnimationComponent::getASM() const{
	return animationFiniteStateMachine;
}

SKA_DEFINE_COMPONENT(ska::AnimationComponent);
