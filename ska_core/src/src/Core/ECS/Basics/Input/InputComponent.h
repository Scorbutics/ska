#pragma once
#include <string>
#include "Base/Values/Strings/StringUtils.h"
#include "Core/ECS/Component.h"
namespace ska {
	class InputComponent {
	public:
	    InputComponent();
	    ~InputComponent() = default;
			float movePower;
			float jumpPower;
	};
}
SKA_DECLARE_COMPONENT(ska::InputComponent);
