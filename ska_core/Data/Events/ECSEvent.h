#pragma once
#include <unordered_set>
#include "../../ECS/ECSDefines.h"

namespace ska {
	enum ECSEventType {
		ENTITIES_REMOVED
	};

	/**
	* \brief Declares an event struct that is sent to observers when the ECS core act on a group of entities
	*/
	struct ECSEvent {
		explicit ECSEvent(const ECSEventType& ecs) : 
			ecsEventType(ecs) {
		}

		const ECSEventType ecsEventType;
		std::unordered_set<ska::EntityId> entities;
	};
}
