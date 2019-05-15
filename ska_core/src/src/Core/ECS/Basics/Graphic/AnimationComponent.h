#pragma once
#include "Base/Values/Strings/StringUtils.h"
#include "../../ECSDefines.h"

#include "Core/ApiDef.h"
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
}

SKA_DECLARE_COMPONENT(ska::AnimationComponent)
