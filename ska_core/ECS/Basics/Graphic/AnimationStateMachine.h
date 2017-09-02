#pragma once
#include "../../ECSDefines.h"

namespace ska {
	class AnimationComponent;

	class AnimationStateMachine {
	public:
		virtual ~AnimationStateMachine() = default;
		virtual void onEnter() = 0;
		virtual AnimationStateMachine* animate(AnimationComponent&, EntityId&) = 0;
	};
}
