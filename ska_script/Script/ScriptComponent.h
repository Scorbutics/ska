#pragma once
#include <vector>
#include <limits>
#include <memory>
#include <unordered_map>
#include "ECS/SerializableComponent.h"
#include "ScriptState.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "ECS/ECSDefines.h"
#include "ScriptController.h"

namespace ska {
    class ScriptAutoSystem;
	class ScriptComponent : public SerializableComponent {
		friend class ScriptAutoSystem;
		friend class ScriptController;

		//TODO éviter shared_ptr en modifiant le cache des scripts (voir ScriptAutoSystem)
        using ScriptControllerPtr = std::shared_ptr<ScriptController>;

	public:
		ScriptComponent():
			deleteEntityWhenFinished(false),
			scriptPeriod(0),
			triggeringType(0),
			origin(0) {
			state = EnumScriptState::STOPPED;
			lastTimeStarted = 0;
			commandsPlayed = 0;
			lastTimeDelayed = 0;
			delay = 0;
			entityId = std::numeric_limits<unsigned int>().max();
			active = 0;
			parent = nullptr;
		}

		bool deleteEntityWhenFinished;
		long scriptPeriod;
		std::string context;
		std::string extendedName;
		std::vector<std::string> extraArgs;
		ScriptTriggerType triggeringType;
		ScriptAutoSystem* parent;
		ScriptControllerPtr controller;
		int active;
		std::unordered_map<std::string, std::string> varMap;
		std::string fullPath;
		std::string key;

		EntityId getEntity() const {
			return entityId;
		}

		EntityId getOrigin() const {
			return origin;
		}

	private:
		/* Manage states in order to dispatch all different scripts */
		ScriptState state;
		unsigned int lastTimeStarted;
		unsigned int commandsPlayed;
		std::string lastResult;
		unsigned int lastTimeDelayed;
		unsigned int delay;
		EntityId entityId;
		EntityId origin;
	};
}
