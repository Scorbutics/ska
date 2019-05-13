#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "ScriptAwareComponent.h"

ska::ScriptAwareComponent::ScriptAwareComponent() {}

std::size_t ska::ComponentTag<ska::ScriptAwareComponent>::m_id = static_cast<std::size_t>(-1);