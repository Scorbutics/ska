#pragma once

#include "../../ECS/ECSDefines.h"
#include "../../ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	enum class ScriptEventType {
		EntityChangeBlockId,
		EntityChangeBlockProperty,
		ScriptCreate
	};

	struct ScriptEvent {
		std::vector<ScriptEventType> type;
		ScriptSleepComponent scriptSleep;
		const EntityId parent;
		const Point<int> oldBlockId;
		const Point<int> newBlockId;
		const Point<int> oldPos;
		const Point<int> newPos;
	};
}