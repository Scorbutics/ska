
#include "GraphicComponent.h"

ska::GraphicComponent::GraphicComponent() {
    desiredPriority = std::numeric_limits<int>().min();
}

ska::GraphicComponent& ska::GraphicComponent::operator=(const GraphicComponent& gc) {
	animatedSprites = gc.animatedSprites;
	sprites = gc.sprites;
    desiredPriority = gc.desiredPriority;
	m_drawn = gc.m_drawn;
	return *this;
}

ska::Interpolated::Value ska::GraphicComponent::drawnPosition(const PositionComponent& position) {
	constexpr auto snapDistance = 96.F;
	return m_drawn.sample({ static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(position.z) }, snapDistance);
}

SKA_DEFINE_COMPONENT(ska::GraphicComponent);
