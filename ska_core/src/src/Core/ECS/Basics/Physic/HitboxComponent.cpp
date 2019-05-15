#define SKA_IN_COMPONENT
#include "HitboxComponent.h"

ska::HitboxComponent::HitboxComponent(): 
	xOffset(0), 
	yOffset(0), 
	width(1), 
	height(1) {
}

SKA_DEFINE_COMPONENT(ska::HitboxComponent);
SKA_DEFINE_COMPONENT(ska::cp::HitboxComponent);
