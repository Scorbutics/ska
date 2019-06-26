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

	private:
		IAnimationStateMachine* animationFiniteStateMachine;		
	};
}

SKA_DECLARE_COMPONENT(ska::AnimationComponent)
