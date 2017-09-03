#include "AnimationComponent.h"
#include "IAnimationStateMachine.h"

ska::AnimationComponent::AnimationComponent() :
	animationFiniteStateMachine(nullptr),
    state(0) {
    /*type(DirectionalAnimationType::MOVEMENT), looked(0) {
    static auto initialized = false;
    if (!initialized) {
        initialized = true;
        const auto className = ComponentHandler<std::remove_reference<decltype(*this)>::type>::getClassName();
        addFieldSerializer(serializeDirection, "direction", className);
    }
    direction = 0;*/
}

void ska::AnimationComponent::setASM(IAnimationStateMachine* afsm, EntityId& entityId){
	animationFiniteStateMachine = afsm;
	animationFiniteStateMachine->onEnter(entityId);
}

ska::IAnimationStateMachine* ska::AnimationComponent::getASM() const{
	return animationFiniteStateMachine;
}
