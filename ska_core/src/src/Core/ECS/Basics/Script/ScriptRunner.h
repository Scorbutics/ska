#pragma once
#include "ScriptSleepComponent.h"
#include "Core/ECS/ECSDefines.h"

namespace ska {
	// Runs the scripts of a scripting system (ex : skalang scripts), started by the engine script triggers
	class ScriptRunner {
	public:
		virtual ~ScriptRunner() = default;
		// Starts a script, triggered by the "origin" entity
		virtual void start(const ScriptSleepComponent& script, EntityId origin) = 0;
		// Called at each frame
		virtual void update(unsigned int ellapsedTime) = 0;
	};
}
