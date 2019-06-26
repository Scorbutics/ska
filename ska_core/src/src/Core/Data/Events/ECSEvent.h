#pragma once
#include <unordered_map>
#include "../../ECS/ECSDefines.h"

namespace ska {
	enum class ECSEventType {
		ENTITIES_REMOVED,
		ENTITIES_ADDED
	};

	/**
	* \brief Declares an event struct that is sent to observers when the ECS core act on a group of entities
	*/
	struct ECSEvent {
		explicit ECSEvent(const ECSEventType& ecs) : 
			ecsEventType(ecs) {
		}

		const ECSEventType ecsEventType;
		std::vector<EntityId> entities;
	};
}
