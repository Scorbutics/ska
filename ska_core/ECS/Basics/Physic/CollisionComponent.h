#pragma once
#include "../../SerializableComponent.h"
#include "../../ECSDefines.h"
#include "CollisionContact.h"

namespace ska {
	class CollisionComponent : public SerializableComponent {
	public:
		CollisionComponent();
		EntityId origin;
		EntityId target;
		bool xaxis;
		bool yaxis;
		CollisionContact contact;
	};
}
