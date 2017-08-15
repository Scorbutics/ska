#pragma once
#include "../../Component.h"
#include "../../../Point.h"

namespace ska {
	class CameraFocusedComponent :
            public Component {
    public:
        CameraFocusedComponent();
        ~CameraFocusedComponent() = default;
	};
}
