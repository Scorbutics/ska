#include "AnimationComponent.h"
#include "IAnimationStateMachine.h"
#include "../../SerializeComponent.h"

ska::AnimationComponent::AnimationComponent() :
	state(0),
    animationFiniteStateMachine(nullptr) {
	SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldDeserializer(deserializeState, "state");
	SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeState, "state");
}

ska::AnimationComponent::~AnimationComponent(){}

void ska::AnimationComponent::setASM(IAnimationStateMachine& afsm, const EntityId& entityId){
	animationFiniteStateMachine = &afsm;
	animationFiniteStateMachine->onEnter(entityId);
}

ska::IAnimationStateMachine* ska::AnimationComponent::getASM() const{
	return animationFiniteStateMachine;
}

SKA_DECLARE_COMPONENT(ska::AnimationComponent);
