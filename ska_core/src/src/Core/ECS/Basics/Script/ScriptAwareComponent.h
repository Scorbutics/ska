#pragma once
#include "../Physic/PositionComponent.h"
#include "Core/ECS/Component.h"
namespace ska {
	class ScriptAwareComponent {
	public:
	    ScriptAwareComponent();
	    ~ScriptAwareComponent() = default;

		PositionComponent lastBlockPos;
		// The block the entity entered last (block coordinates) : the scripts of a block "under the entity" (MOVE_IN) run
		// once, when it enters the block (ex : a door). A game placing the entity on a block without running them (ex : the
		// player coming out of a door) sets it.
		Point<int> enteredBlock { -1, -1 };
	};
}
SKA_DECLARE_COMPONENT(ska::ScriptAwareComponent);