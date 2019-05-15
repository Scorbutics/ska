#pragma once
#include "../Physic/PositionComponent.h"
#include "Core/ECS/Component.h"
namespace ska {
	class ScriptAwareComponent {
	public:
	    ScriptAwareComponent();
	    ~ScriptAwareComponent() = default;

		PositionComponent lastBlockPos;
	};
}
SKA_DECLARE_COMPONENT(ska::ScriptAwareComponent);