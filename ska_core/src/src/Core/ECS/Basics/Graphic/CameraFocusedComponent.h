#pragma once
#include "Core/ECS/Component.h"

namespace ska {
	class CameraFocusedComponent {
    public:
        CameraFocusedComponent();
        ~CameraFocusedComponent() = default;
	};
}
SKA_DECLARE_COMPONENT(ska::CameraFocusedComponent);
