#pragma once
#include "../../SerializableComponent.h"
#include "../../../Point.h"

namespace ska {
	class CameraFocusedComponent :
            public SerializableComponent {
    public:
        CameraFocusedComponent();
        ~CameraFocusedComponent() = default;
	};
}
