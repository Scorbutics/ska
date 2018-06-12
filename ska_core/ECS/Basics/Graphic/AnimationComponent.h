#pragma once

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

	private:
		IAnimationStateMachine* animationFiniteStateMachine;
		//int direction;
		//DirectionalAnimationType::Enum type;
		/*EntityId looked;

	protected:
		static std::string serializeDirection(const Component& component) {
			return StringUtils::intToStr(static_cast<const AnimationComponent&>(component).direction);
		}*/

	};
}
