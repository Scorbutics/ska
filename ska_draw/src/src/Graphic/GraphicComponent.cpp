#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
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

template class SKA_CORE_API ska::ComponentTag<ska::GraphicComponent>;
std::size_t ska::ComponentTag<ska::GraphicComponent>::m_id = static_cast<std::size_t>(-1);
