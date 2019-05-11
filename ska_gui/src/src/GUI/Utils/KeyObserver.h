#pragma once
#include "Base/Patterns/Observer.h"
#include "../Events/KeyEvent.h"

namespace ska {
	using KeyObserver = Observer<KeyEvent>;
}
