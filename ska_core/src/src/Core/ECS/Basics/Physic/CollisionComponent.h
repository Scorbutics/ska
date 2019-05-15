#pragma once
#include "../../ECSDefines.h"
#include "CollisionContact.h"
#include "Core/ApiDef.h"
#include "Core/ECS/Component.h"

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
}
SKA_DECLARE_COMPONENT(ska::CollisionComponent);