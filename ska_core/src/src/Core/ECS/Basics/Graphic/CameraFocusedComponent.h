#pragma once
#include "Core/ECS/Component.h"

namespace ska {
	class CameraFocusedComponent : public Component<CameraFocusedComponent> {
    public:
        CameraFocusedComponent();
        ~CameraFocusedComponent() = default;
	};
}
