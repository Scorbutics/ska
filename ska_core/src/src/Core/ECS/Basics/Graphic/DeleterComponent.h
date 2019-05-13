#pragma once
#include "Core/ECS/ComponentTag.h"

namespace ska {
	class DeleterComponent {
	public:
		DeleterComponent();

		int delay;
		unsigned int startTime;
	};
#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<DeleterComponent>;
#endif
}
