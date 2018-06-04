#pragma once
#include <unordered_set>
#include <string>
#include <unordered_map>
#include "../Command/Command.h"
#include "Data/MemoryScript.h"
#include "../ScriptComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	class TileWorld;

	class ScriptAutoSystem :
		public System< RequiredComponent<ScriptComponent>, PossibleComponent<ScriptSleepComponent, ScriptComponent>> {
	public:

		MemoryScript& getSavegame();
		void registerScript(ScriptComponent* parent, EntityId scriptSleepEntity, EntityId origin);
		void registerCommand(const std::string& cmdName, CommandPtr& cmd);
		//void setupScriptArgs(ScriptComponent* parent, ScriptComponent& script, const std::vector<std::string>& args);
		//void kill(const std::string& keyScript);
		virtual std::string map(const std::string& key, const std::string& id) const;
		void registerNamedScriptedEntity(const std::string& nameEntity, EntityId entity);
		void clearNamedScriptedEntities();
		void removeComponent(const std::string& componentName, const std::string& id) const;
		void restoreComponent(const std::string& componentName, const std::string& id) const;
		EntityId getEntityFromName(const std::string& nameEntity);

		/* ScriptComponent methods */
		float getPriority(ScriptComponent& script, unsigned int currentTimeMillis);
		bool canBePlayed(ScriptComponent& script);
		bool transferActiveToDelay(ScriptComponent& script);
		bool play(ScriptComponent& script, MemoryScript& savegame);
		void killAndSave(ScriptComponent& script, const MemoryScript& savegame) const;
		ScriptState manageCurrentState(ScriptComponent& script);
		std::string interpret(ScriptComponent& script, MemoryScript& savegame, const std::string& cmd);
		void stop(ScriptComponent& script);

		void operator=(const ScriptAutoSystem&) = delete;

		virtual ~ScriptAutoSystem();

	private:
		MemoryScript& m_saveGame;
		ScriptComponent* getHighestPriorityScript();

		std::unordered_map<std::string, ScriptComponent> m_cache;
		std::unordered_map<std::string, CommandPtr> m_commands;
		std::unordered_map<std::string, EntityId> m_namedScriptedEntities;

	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
		struct ScriptCommandHelper {
			ScriptCommandHelper(EntityManager& parent) : m_entityManager(parent) {}
			virtual ~ScriptCommandHelper() = default;
			void operator=(const ScriptCommandHelper&) = delete;
			virtual void setupCommands(std::unordered_map<std::string, CommandPtr>& commands) const = 0;
			EntityManager& m_entityManager;
		};

		ScriptAutoSystem(EntityManager& entityManager, const ScriptCommandHelper& sch, MemoryScript& saveGame);
	};

}
