#define SKA_IN_COMPONENT
#include "DeleterComponent.h"
#include "../../../Utils/TimeUtils.h"

ska::DeleterComponent::DeleterComponent() : delay(0), startTime(TimeUtils::getTicks()) { }

SKA_DEFINE_COMPONENT(ska::DeleterComponent);
