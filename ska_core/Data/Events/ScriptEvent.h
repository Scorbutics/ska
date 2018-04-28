#pragma once

#include "../../ECS/ECSDefines.h"

namespace ska {
	enum class ScriptEventType {
		EntityChangeBlockId
	};

	struct ScriptEvent {
		const ScriptEventType type;
		const EntityId entityId;
		const Point<int> oldBlockId;
		const Point<int> newBlockId;
		const Point<int> oldPos;
		const Point<int> newPos;
	};
}