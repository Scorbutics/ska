#pragma once
#include "Core/ECS/ComponentTag.h"
#include "Base/Values/Strings/StringUtils.h"
#include "../../ECSDefines.h"

namespace ska {

	class IAnimationStateMachine;
	class AnimationComponent {
	public:
		AnimationComponent();
		~AnimationComponent();

		void setASM(IAnimationStateMachine& afsm, const EntityId& entity);
		IAnimationStateMachine* getASM() const;

		long state;

		static std::string getClassName() {
			return "AnimationComponent";
		}

	private:
		IAnimationStateMachine* animationFiniteStateMachine;

		static std::string serializeState(const AnimationComponent& component) {
			return StringUtils::toString(component.state);
		}

		static void deserializeState(AnimationComponent& component, const std::string& value) {
			component.state = StringUtils::fromString<long>(value);
		}
		
	};

#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<AnimationComponent>;
#endif
}
