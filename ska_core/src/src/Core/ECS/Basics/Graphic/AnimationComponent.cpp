#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
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

std::size_t ska::ComponentTag<ska::AnimationComponent>::m_id = static_cast<std::size_t>(-1);
