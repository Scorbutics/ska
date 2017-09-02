#pragma once
#include "../../Component.h"
#include "../../../Utils/StringUtils.h"
namespace ska {
		
	class AnimationStateMachine;
	class AnimationComponent : 
		public Component {
	public:
		AnimationComponent();
		~AnimationComponent() = default;

		void setASM(AnimationStateMachine* afsm);
		AnimationStateMachine* getASM() const;

		long state;

	private:
		AnimationStateMachine* animationFiniteStateMachine;
		//int direction;
		//DirectionalAnimationType::Enum type;
		/*EntityId looked;

	protected:
		static std::string serializeDirection(const Component& component) {
			return StringUtils::intToStr(static_cast<const AnimationComponent&>(component).direction);
		}*/

	};
}
