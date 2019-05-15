#pragma once
#include "Core/ECS/Component.h"

namespace ska {
	class DeleterComponent {
	public:
		DeleterComponent();

		int delay;
		unsigned int startTime;
	};
}
SKA_DECLARE_COMPONENT(ska::DeleterComponent);
