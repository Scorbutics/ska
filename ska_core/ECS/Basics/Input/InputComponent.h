#pragma once
#include "../../SerializableComponent.h"

namespace ska {
	class InputComponent : public SerializableComponent {
	public:
	    InputComponent();
	    ~InputComponent() = default;
		float movePower;
		float jumpPower;
	};
}
