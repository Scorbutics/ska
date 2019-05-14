#include "Core/__internalConfig/LoggerConfig.h"
#include "CollisionComponent.h"

ska::CollisionComponent::CollisionComponent() {
	xaxis = false;
	yaxis = false;
	origin = 0;
	target = 0;
}

SKA_DECLARE_COMPONENT(ska::CollisionComponent);
