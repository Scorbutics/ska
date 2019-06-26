
#include "PositionComponent.h"

ska::PositionComponent::PositionComponent() {
}

ska::PositionComponent::PositionComponent(const Point<float>& p) {
    operator=(p);
}

ska::PositionComponent::PositionComponent(const Point<int>& p) {
	operator=(ska::Point<float>(static_cast<float>(p.x), static_cast<float>(p.y)));
}

ska::PositionComponent& ska::PositionComponent::operator=(const Point<float>& p) {
    x = static_cast<decltype(x)>(p.x);
    y = static_cast<decltype(y)>(p.y);
    z = 0;
	return *this;
}

SKA_DEFINE_COMPONENT(ska::PositionComponent);
