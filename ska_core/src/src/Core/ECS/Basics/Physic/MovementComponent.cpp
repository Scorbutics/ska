#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "MovementComponent.h"

ska::MovementComponent::MovementComponent() {
    ax = 0;
    ay = 0;
    az = 0;

    vx = 0;
    vy = 0;
    vz = 0;
}
template class SKA_CORE_API ska::ComponentTag<ska::MovementComponent>;
std::size_t ska::ComponentTag<ska::MovementComponent>::m_id = static_cast<std::size_t>(-1);
