#pragma once
#include "../../ECSDefines.h"

namespace ska {
	class AnimationComponent;

	class IAnimationStateMachine {
	public:
		virtual ~IAnimationStateMachine() = default;
		virtual IAnimationStateMachine* animate(AnimationComponent& ac, const EntityId& e) = 0;
		virtual void onEnter(const EntityId&) = 0;
	};
}
