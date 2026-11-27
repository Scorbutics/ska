#pragma once
#include "Base/Values/Strings/StringUtils.h"
#include "../../ECSDefines.h"

#include "Core/ECS/Component.h"
namespace ska {

	class IAnimationStateMachine;
	class AnimationComponent {
	public:
		AnimationComponent();
		~AnimationComponent();

		void setASM(IAnimationStateMachine& afsm, const EntityId& entity);
		IAnimationStateMachine* getASM() const;

		long state;

		// Set by a movement system (ex : block by block) : the character walks ("walking") in the direction "state", with
		// a frame each "walkDelay" ms, whatever its physics velocity. Valid until "drivenUntil" (ticks)
		unsigned int drivenUntil = 0;
		bool walking = false;
		unsigned int walkDelay = 0;
		// The animation goes on when the character does not move (ex : bobbing on the water)
		bool animatedWhenStill = false;
		// The walk animation is running (its first frame is shown when it starts)
		bool walkShown = false;
		// The column of the single frame shown by PoseAnimationStateMachine (a pose between two animations, ex : setting
		// off), -1 : none
		int pose = -1;

	private:
		IAnimationStateMachine* animationFiniteStateMachine;		
	};
}

SKA_DECLARE_COMPONENT(ska::AnimationComponent)
