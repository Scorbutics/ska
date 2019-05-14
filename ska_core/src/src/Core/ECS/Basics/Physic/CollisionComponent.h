#pragma once
#include "../../ECSDefines.h"
#include "CollisionContact.h"
#include "Core/ECS/Component.h"

namespace ska {
	class CollisionComponent : public Component<CollisionComponent> {
	public:
		CollisionComponent();
		EntityId origin;
		EntityId target;
		bool xaxis;
		bool yaxis;
		CollisionContact contact;
	};
}
