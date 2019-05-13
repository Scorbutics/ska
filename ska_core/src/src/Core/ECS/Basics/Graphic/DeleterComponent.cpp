#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "DeleterComponent.h"
#include "../../../Utils/TimeUtils.h"

ska::DeleterComponent::DeleterComponent() : delay(0), startTime(TimeUtils::getTicks()) { }

template class SKA_CORE_API ska::ComponentTag<ska::DeleterComponent>;
std::size_t ska::ComponentTag<ska::DeleterComponent>::m_id = static_cast<std::size_t>(-1);
