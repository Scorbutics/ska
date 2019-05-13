#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "HitboxComponent.h"

ska::HitboxComponent::HitboxComponent(): 
	xOffset(0), 
	yOffset(0), 
	width(1), 
	height(1) {
}

template class SKA_CORE_API ska::ComponentTag<ska::HitboxComponent>;
template class SKA_CORE_API ska::ComponentTag<ska::cp::HitboxComponent>;
std::size_t ska::ComponentTag<ska::HitboxComponent>::m_id = static_cast<std::size_t>(-1);
std::size_t ska::ComponentTag<ska::cp::HitboxComponent>::m_id = static_cast<std::size_t>(-1);
