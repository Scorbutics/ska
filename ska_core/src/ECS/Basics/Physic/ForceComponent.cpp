#include "ForceComponent.h"

ska::ForceComponent::ForceComponent() : 
	bounciness(1.F),
	maxSpeed(480.F) {
    x = 0;
    y = 0;
    z = 0;
    weight = 1;
}
