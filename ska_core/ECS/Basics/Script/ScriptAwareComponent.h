#pragma once
#include "../../SerializableComponent.h"
#include "../Physic/PositionComponent.h"

namespace ska {
	class ScriptAwareComponent : public SerializableComponent {
	public:
	    ScriptAwareComponent();
	    ~ScriptAwareComponent() = default;

		PositionComponent lastBlockPos;
	};
}
