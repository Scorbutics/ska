#define SKA_COMPONENT_DLL_EXPORT
#define SKA_CORE_IN
#include "ScriptSleepComponent.h"

ska::ScriptSleepComponent::ScriptSleepComponent():
	id(0), triggeringType(ScriptTriggerType::NONE)
{
	deleteEntityWhenFinished = false;
	period = 0;
}

std::size_t ska::ComponentTag<ska::ScriptSleepComponent>::m_id = static_cast<std::size_t>(-1);