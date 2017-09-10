#pragma once
#include <unordered_set>
#include "../../ECS/ECSDefines.h"

namespace ska {
	enum ECSEventType {
		ENTITIES_REMOVED
	};

	struct ECSEvent {
		explicit ECSEvent(const ECSEventType& ecs) : 
			ecsEventType(ecs) {
		}

		const ECSEventType ecsEventType;
		std::unordered_set<ska::EntityId> entities;
	};
}
