#include "PositionComponent.h"

ska::PositionComponent::PositionComponent() {
    static auto initialized = false;
    if (!initialized) {
        initialized = true;
        SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeX, "x");
        SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeY, "y");
        SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeZ, "z");
    }
}

ska::PositionComponent::PositionComponent(const Point<float>& p) {
    operator=(p);
}

ska::PositionComponent::PositionComponent(const Point<int>& p) {
	operator=(ska::Point<float>(p.x, p.y));
}

ska::PositionComponent& ska::PositionComponent::operator=(const Point<float>& p) {
    x = p.x;
    y = p.y;
    z = 0;
	return *this;
}

