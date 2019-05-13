#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "DeleterComponent.h"
#include "../../../Utils/TimeUtils.h"

ska::DeleterComponent::DeleterComponent() : delay(0), startTime(TimeUtils::getTicks()) { }

std::size_t ska::ComponentTag<ska::DeleterComponent>::m_id = static_cast<std::size_t>(-1);
