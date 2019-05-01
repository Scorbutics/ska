#include "DeleterComponent.h"
#include "../../../Utils/TimeUtils.h"

ska::DeleterComponent::DeleterComponent() : delay(0), startTime(TimeUtils::getTicks()) {
}
