#pragma once
#include "../../SerializableComponent.h"
#include "../../../Utils/TimeUtils.h"

namespace ska {
	class DeleterComponent: public SerializableComponent {
	public:
		DeleterComponent();

		int delay;
		unsigned int startTime;
	};
}
