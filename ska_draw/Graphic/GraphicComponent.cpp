#include "GraphicComponent.h"

ska::GraphicComponent::GraphicComponent() {
    desiredPriority = std::numeric_limits<int>().min();
}

ska::GraphicComponent& ska::GraphicComponent::operator=(const GraphicComponent& gc) {
	animatedSprites = gc.animatedSprites;
	sprites = gc.sprites;
    desiredPriority = gc.desiredPriority;
	return *this;
}

