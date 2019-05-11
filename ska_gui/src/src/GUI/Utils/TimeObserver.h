#pragma once
#include "Base/Patterns/Observer.h"
#include "../Events/TimeEvent.h"

namespace ska {
	using TimeObserver = Observer<TimeEvent>;
}
