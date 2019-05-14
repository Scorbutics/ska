#include "GravityAffectedComponent.h"

ska::GravityAffectedComponent::GravityAffectedComponent(): 
	friction(20.F), rotationFriction(20.F) {
}

SKA_DECLARE_COMPONENT(ska::GravityAffectedComponent);
