#pragma once
#include "../Physic/PositionComponent.h"
#include "Core/ECS/ComponentTag.h"
namespace ska {
	class ScriptAwareComponent {
	public:
	    ScriptAwareComponent();
	    ~ScriptAwareComponent() = default;

		PositionComponent lastBlockPos;
	};
#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<ScriptAwareComponent>;
#endif
}
