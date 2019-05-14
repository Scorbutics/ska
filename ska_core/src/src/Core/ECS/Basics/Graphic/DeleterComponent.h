#pragma once
#include "Core/ECS/Component.h"

namespace ska {
	class DeleterComponent : public Component<DeleterComponent> {
	public:
		DeleterComponent();

		int delay;
		unsigned int startTime;
	};
}
