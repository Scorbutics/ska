#define SKA_IN_COMPONENT
#include "GravityAffectedComponent.h"

ska::GravityAffectedComponent::GravityAffectedComponent(): 
	friction(20.F), rotationFriction(20.F) {
}

SKA_DEFINE_COMPONENT(ska::GravityAffectedComponent);
