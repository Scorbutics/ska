#pragma once
#include "../../ECSDefines.h"

namespace ska {
	class AnimationComponent;

	class IAnimationStateMachine {
	public:
		virtual ~IAnimationStateMachine() = default;
		virtual IAnimationStateMachine* animate(AnimationComponent& ac, EntityId& e) = 0;
		virtual void onEnter(EntityId&) = 0;
	};
}
