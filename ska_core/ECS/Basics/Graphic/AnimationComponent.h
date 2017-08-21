#pragma once
#include "../../Component.h"
#include "../../../Utils/StringUtils.h"
namespace ska {
	class AnimationComponent : public Component {
	public:
		AnimationComponent();
        ~AnimationComponent() = default;

		long state;
		int indexAnimationFiniteStateMachine;
		//int direction;
		//DirectionalAnimationType::Enum type;
		/*EntityId looked;

	protected:
		static std::string serializeDirection(const Component& component) {
			return StringUtils::intToStr(static_cast<const AnimationComponent&>(component).direction);
		}*/

	};
}
