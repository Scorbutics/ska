#include "ScriptSleepComponent.h"

ska::ScriptSleepComponent::ScriptSleepComponent():
	id(0), triggeringType(ScriptTriggerType::NONE)
{
	deleteEntityWhenFinished = false;
	period = 0;
}

SKA_DECLARE_COMPONENT(ska::ScriptSleepComponent);
