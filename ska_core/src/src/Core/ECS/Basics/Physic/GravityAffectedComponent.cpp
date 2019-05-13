#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "GravityAffectedComponent.h"

ska::GravityAffectedComponent::GravityAffectedComponent(): 
	friction(20.F), rotationFriction(20.F) {
}

std::size_t ska::ComponentTag<ska::GravityAffectedComponent>::m_id = static_cast<std::size_t>(-1);
