#pragma once
#include <unordered_set>
#include <optional>
#include <string>
#include <unordered_map>

#include "../Command/Command.h"
#include "../ScriptComponent.h"

#include "Core/Data/MemoryScript.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Core/ECS/EntityLocator.h"

//skalang
#include "Service/ReservedKeywordsPool.h"
#include "Service/Tokenizer.h"
#include "Interpreter/Value/Script.h"
#include "Service/StatementParser.h"
#include "Service/SemanticTypeChecker.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "Service/TypeBuilder/TypeBuildUnit.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Interpreter/Interpreter.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"

//skalang std lib
#include "std/module.h"
#include "std/module/function/parameter.h"

namespace ska {
	class TileWorld;

	class ScriptAutoSystem :
		public System< RequiredComponent<ScriptComponent>, PossibleComponent<ScriptSleepComponent, ScriptComponent>> {
	public:

		MemoryScript& getSavegame();
		void registerScript(const ScriptSleepComponent& scriptData, EntityId origin, std::optional<EntityId> target = {});
		void registerCommand(const std::string& cmdName, CommandPtr& cmd);
		//void setupScriptArgs(ScriptComponent* parent, ScriptComponent& script, const std::vector<std::string>& args);
		//void kill(const std::string& keyScript);
		virtual std::string map(const std::string& key, EntityId id) const;

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

		virtual ~ScriptAutoSystem() = default;

	private:
	  const ReservedKeywordsPool reservedKeywords;
		ScriptCache scriptCache;
		TypeCrosser typeCrosser;
		StatementParser parser;
		TypeBuilder typeBuilder;
		SymbolTableTypeUpdater symbolsTypeUpdater;
		SemanticTypeChecker typeChecker;
		Interpreter interpreter;
		lang::ModuleConfiguration moduleConfiguration;

		EntityManager& m_entityManager;
		MemoryScript& m_saveGame;
		ScriptComponent* getHighestPriorityScript();

		std::unordered_map<std::string, ScriptComponent> m_cache;
		std::unordered_map<std::string, CommandPtr> m_commands;

		std::vector<ScriptComponent> m_componentToAddQueue;

	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
		struct ScriptCommandHelper {
			ScriptCommandHelper(EntityManager& parent) : m_entityManager(parent) {}
			virtual ~ScriptCommandHelper() = default;
			void operator=(const ScriptCommandHelper&) = delete;
			virtual void setupCommands(std::unordered_map<std::string, CommandPtr>& commands) const = 0;
			EntityManager& m_entityManager;
		};

		ScriptAutoSystem(EntityManager& entityManager, const ScriptCommandHelper& sch, MemoryScript& saveGame, GameEventDispatcher& ged);
	};

}
