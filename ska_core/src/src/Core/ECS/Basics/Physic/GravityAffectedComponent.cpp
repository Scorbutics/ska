#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "GravityAffectedComponent.h"

ska::GravityAffectedComponent::GravityAffectedComponent(): 
	friction(20.F), rotationFriction(20.F) {
}

template class SKA_CORE_API ska::ComponentTag<ska::GravityAffectedComponent>;
std::size_t ska::ComponentTag<ska::GravityAffectedComponent>::m_id = static_cast<std::size_t>(-1);
