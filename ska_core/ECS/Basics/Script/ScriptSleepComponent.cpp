#include "ScriptSleepComponent.h"

ska::ScriptSleepComponent::ScriptSleepComponent(): 
	id(0) {
	deleteEntityWhenFinished = false;
	triggeringType = 0;
	period = 0;
}
