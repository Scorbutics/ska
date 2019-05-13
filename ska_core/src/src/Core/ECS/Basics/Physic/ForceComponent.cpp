#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "ForceComponent.h"

ska::ForceComponent::ForceComponent() : 
	bounciness(1.F),
	maxSpeed(480.F) {
    x = 0;
    y = 0;
    z = 0;
    weight = 1;
}

std::size_t ska::ComponentTag<ska::ForceComponent>::m_id = static_cast<std::size_t>(-1);
