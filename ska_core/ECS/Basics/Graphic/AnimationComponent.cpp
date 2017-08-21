#include "AnimationComponent.h"

ska::AnimationComponent::AnimationComponent() :
    indexAnimationFiniteStateMachine(0),
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
