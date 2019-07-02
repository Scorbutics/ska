#pragma once
#include <unordered_set>
#include <optional>
#include <string>
#include <unordered_map>

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

	class ScriptRunnerSystem :
		public System< RequiredComponent<ScriptComponent>, PossibleComponent<ScriptSleepComponent, ScriptComponent>> {
	public:
		ScriptRunnerSystem(EntityManager& entityManager, GameEventDispatcher& ged);
		void registerScript(const ScriptSleepComponent& scriptData, EntityId triggerer, std::optional<EntityId> target = {});

		/* ScriptComponent methods */
		bool play(ScriptComponent& script);
		void stop(ScriptComponent& script);
		void killAndSave(ScriptComponent& script) const;

		ScriptRunnerSystem(const ScriptRunnerSystem&) = delete;
		void operator=(const ScriptRunnerSystem&) = delete;
		
		virtual ~ScriptRunnerSystem() = default;

	private:
		virtual void refresh(unsigned int ellapsedTime) override;
		float getPriority(ScriptComponent& script, unsigned int currentTimeMillis);
		bool canBePlayed(ScriptComponent& script);
		bool transferActiveToDelay(ScriptComponent& script);
		virtual std::string map(const std::string& key, EntityId id) const;
		ScriptState manageCurrentState(ScriptComponent& script);
		std::string interpret(ScriptComponent& script, const std::string& cmd);

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
		ScriptComponent* getHighestPriorityScript();

		std::vector<ScriptComponent> m_componentToAddQueue;

	};

}
