#define SKA_IN_COMPONENT
#include "MovementComponent.h"

ska::MovementComponent::MovementComponent() {
    ax = 0;
    ay = 0;
    az = 0;

    vx = 0;
    vy = 0;
    vz = 0;
}

SKA_DEFINE_COMPONENT(ska::MovementComponent);
