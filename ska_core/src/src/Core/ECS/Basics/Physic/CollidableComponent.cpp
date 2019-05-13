#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "CollidableComponent.h"

ska::CollidableComponent::CollidableComponent(): ghost(false){}

std::size_t ska::ComponentTag<ska::CollidableComponent>::m_id = static_cast<std::size_t>(-1);
