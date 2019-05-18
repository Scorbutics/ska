
#include "PositionComponent.h"
#include "../../SerializeComponent.h"

ska::PositionComponent::PositionComponent() {
    static auto initialized = false;
    if (!initialized) {
        initialized = true;
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldDeserializer(deserializeX, "x");
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldDeserializer(deserializeY, "y");
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldDeserializer(deserializeZ, "z");
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldDeserializer(deserializeLayer, "layer");
        SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeX, "x");
        SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeY, "y");
        SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeZ, "z");
		SerializeComponent<std::remove_reference<decltype(*this)>::type>::addFieldSerializer(serializeLayer, "layer");
    }
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
