#pragma once
#include "Core/ECS/ComponentTag.h"

namespace ska {
	class HasShadowComponent {
	public:
	    HasShadowComponent();
	    ~HasShadowComponent() = default;
	};
#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<HasShadowComponent>;
#endif
}
