#pragma once

#include "Core/ECS/ComponentTag.h"

namespace ska {
	class CameraFocusedComponent {
    public:
        CameraFocusedComponent();
        ~CameraFocusedComponent() = default;
	};

#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<CameraFocusedComponent>;
#endif
}
