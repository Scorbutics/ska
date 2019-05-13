#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "DebugGraphicComponent.h"

ska::DebugGraphicComponent::DebugGraphicComponent() {
    typeMask = 0;
}

std::size_t ska::ComponentTag<ska::DebugGraphicComponent>::m_id = static_cast<std::size_t>(-1);
