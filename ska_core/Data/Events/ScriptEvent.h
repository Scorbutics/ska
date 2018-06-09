#pragma once

#include "../../ECS/ECSDefines.h"

namespace ska {
	enum class ScriptEventType {
		EntityChangeBlockId,
		EntityChangeBlockProperty,
		ScriptCreate
	};

	struct ScriptEvent {
		std::vector<ScriptEventType> type;
		const EntityId entityId;
		const EntityId parent;
		const Point<int> oldBlockId;
		const Point<int> newBlockId;
		const Point<int> oldPos;
		const Point<int> newPos;
	};
}