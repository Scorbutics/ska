#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "CameraFocusedComponent.h"

ska::CameraFocusedComponent::CameraFocusedComponent() { }

template class SKA_CORE_API ska::ComponentTag<ska::CameraFocusedComponent>;
std::size_t ska::ComponentTag<ska::CameraFocusedComponent>::m_id = static_cast<std::size_t>(-1);

