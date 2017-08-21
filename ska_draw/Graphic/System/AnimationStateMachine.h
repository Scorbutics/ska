#pragma once
#include "ECS/ECSDefines.h"

namespace ska {
    class AnimationComponent;

	class AnimationStateMachine {
	public:
		AnimationStateMachine() = default;
		virtual ~AnimationStateMachine() = default;

		virtual void animate(EntityId& entityId, AnimationComponent& ac) = 0;
	};
}

