#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "HasShadowComponent.h"

ska::HasShadowComponent::HasShadowComponent() {
}
template class SKA_CORE_API ska::ComponentTag<ska::HasShadowComponent>;
std::size_t ska::ComponentTag<ska::HasShadowComponent>::m_id = static_cast<std::size_t>(-1);
