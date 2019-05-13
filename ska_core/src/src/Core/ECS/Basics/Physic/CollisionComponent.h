#pragma once
#include "../../ECSDefines.h"
#include "CollisionContact.h"
#include "Core/ECS/ComponentTag.h"

namespace ska {
	class CollisionComponent {
	public:
		CollisionComponent();
		EntityId origin;
		EntityId target;
		bool xaxis;
		bool yaxis;
		CollisionContact contact;
	};

#ifndef SKA_CORE_IN
	extern template class SKA_CORE_API ComponentTag<CollisionComponent>;
#endif
}
