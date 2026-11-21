#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "../../ECS/ECSDefines.h"

namespace ska {
	enum class ECSEventType {
		ENTITIES_REMOVED,
		ENTITIES_ADDED,
		// An entity was given a name (to be found by name, ex : from scripts)
		ENTITY_NAMED
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
		// ENTITY_NAMED : name of each entity of "entities"
		std::vector<std::string> names;
	};
}
