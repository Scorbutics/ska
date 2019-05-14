#pragma once
#include "../Physic/PositionComponent.h"
#include "Core/ECS/Component.h"
namespace ska {
	class ScriptAwareComponent : public Component<ScriptAwareComponent> {
	public:
	    ScriptAwareComponent();
	    ~ScriptAwareComponent() = default;

		PositionComponent lastBlockPos;
	};
}
