#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "ScriptAwareComponent.h"

ska::ScriptAwareComponent::ScriptAwareComponent() {}

template class SKA_CORE_API ska::ComponentTag<ska::ScriptAwareComponent>;
std::size_t ska::ComponentTag<ska::ScriptAwareComponent>::m_id = static_cast<std::size_t>(-1);