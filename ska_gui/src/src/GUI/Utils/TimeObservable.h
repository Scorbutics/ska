#pragma once
#include "Base/Patterns/Observable.h"
#include "../Events/TimeEvent.h"

namespace ska {
	using TimeObservable = Observable<TimeEvent>;
}
