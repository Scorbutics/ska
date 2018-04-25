#pragma once
#include "../Physic/PositionComponent.h"

namespace ska {
	class ScriptAwareComponent {
	public:
	    ScriptAwareComponent();
	    ~ScriptAwareComponent() = default;

		PositionComponent lastBlockPos;
	};
}
