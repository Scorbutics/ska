#include "HitboxComponent.h"

ska::HitboxComponent::HitboxComponent(): 
	xOffset(0), 
	yOffset(0), 
	width(1), 
	height(1) {
}

SKA_DECLARE_COMPONENT(ska::HitboxComponent);
SKA_DECLARE_COMPONENT(ska::cp::HitboxComponent);
