#include "AnimationComponent.h"
#include "AnimationStateMachine.h"

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

void ska::AnimationComponent::setASM(AnimationStateMachine* afsm){
	animationFiniteStateMachine = afsm;
	animationFiniteStateMachine->onEnter();
}

ska::AnimationStateMachine* ska::AnimationComponent::getASM() const{
	return animationFiniteStateMachine;
}
