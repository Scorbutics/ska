
#include "GraphicComponent.h"

ska::GraphicComponent::GraphicComponent() {
    desiredPriority = std::numeric_limits<int>().min();
}

ska::GraphicComponent& ska::GraphicComponent::operator=(const GraphicComponent& gc) {
	animatedSprites = gc.animatedSprites;
	sprites = gc.sprites;
    desiredPriority = gc.desiredPriority;
	m_drawn = gc.m_drawn;
	effect = gc.effect;
	drawOffset = gc.drawOffset;
	return *this;
}

ska::Point<int> ska::GraphicComponent::spriteOrigin(const long x, const long y, const HitboxComponent& hitbox) const {
	return { static_cast<int>(x) - static_cast<int>(hitbox.width / 2) + hitbox.xOffset / 2 + drawOffset.x,
		static_cast<int>(y) - static_cast<int>(hitbox.height / 2) - 5 + drawOffset.y };
}

ska::Interpolated::Value ska::GraphicComponent::drawnPosition(const PositionComponent& position) {
	constexpr auto snapDistance = 96.F;
	return m_drawn.sample({ static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(position.z) }, snapDistance);
}

SKA_DEFINE_COMPONENT(ska::GraphicComponent);
