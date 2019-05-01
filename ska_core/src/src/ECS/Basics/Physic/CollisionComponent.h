#pragma once
#include "../../ECSDefines.h"
#include "CollisionContact.h"

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
