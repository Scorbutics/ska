#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "DialogComponent.h"

ska::DialogComponent::DialogComponent() : handle(nullptr) { }

std::size_t ska::ComponentTag<ska::DialogComponent>::m_id = static_cast<std::size_t>(-1);
