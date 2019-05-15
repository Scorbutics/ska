#pragma once
#include <string>
#include "Base/Values/Strings/StringUtils.h"
#include "Core/ApiDef.h"
#include "Core/ECS/Component.h"
namespace ska {
	class InputComponent {
	public:
	    InputComponent();
	    ~InputComponent() = default;
		float movePower;
		float jumpPower;

		static std::string getClassName() {
			return "InputComponent";
		}

		static std::string serializeMovePower(const InputComponent& component) {
			return StringUtils::intToStr(static_cast<int>(component.movePower));
		}

		static std::string serializeJumpPower(const InputComponent& component) {
			return StringUtils::intToStr(static_cast<int>(component.jumpPower));
		}

		static void deserializeMovePower(InputComponent& component, const std::string& value) {
			component.movePower = StringUtils::fromString<float>(value);
		}

		static void deserializeJumpPower(InputComponent& component, const std::string& value) {
			component.jumpPower = StringUtils::fromString<float>(value);
		}
	};
}
SKA_DECLARE_COMPONENT(ska::InputComponent);
