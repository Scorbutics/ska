#include "PositionComponent.h"

ska::PositionComponent::PositionComponent() {
    static auto initialized = false;
    if (!initialized) {
        initialized = true;
        const auto className = ComponentHandler<std::remove_reference<decltype(*this)>::type>::getClassName();
        addFieldSerializer(serializeX, "x", className);
        addFieldSerializer(serializeY, "y", className);
        addFieldSerializer(serializeZ, "z", className);
    }
    x = y = z = 0;
}

ska::PositionComponent::PositionComponent(const Point<float>& p) {
    operator=(p);
}

ska::PositionComponent::PositionComponent(const Point<int>& p) {
	operator=(ska::Point<float>(p.x, p.y));
}

void ska::PositionComponent::operator=(const Point<float>& p) {
    x = p.x;
    y = p.y;
    z = 0;
}

