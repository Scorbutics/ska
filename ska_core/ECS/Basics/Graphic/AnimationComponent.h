#pragma once
#include "../../Component.h"
namespace ska {
		
	class IAnimationStateMachine;
	class AnimationComponent : 
		public Component {
	public:
		AnimationComponent();
		~AnimationComponent() = default;

		void setASM(IAnimationStateMachine* afsm, EntityId& entity);
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
