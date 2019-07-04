#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "Script/__internalConfig/LoggerConfig.h"
#include "ScriptRunnerSystem.h"
#include "Core/Exceptions/ScriptDiedException.h"
#include "Core/Exceptions/ScriptUnknownCommandException.h"
#include "Core/Exceptions/IllegalArgumentException.h"
#include "Core/Exceptions/InvalidPathException.h"
#include "Core/Exceptions/NumberFormatException.h"
#include "Core/Exceptions/ScriptSyntaxError.h"
#include "Base/IO/Files/FileUtils.h"
#include "Core/Utils/TimeUtils.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/ECS/Basics/Script/ScriptTriggerType.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Service/ScriptNameBuilder.h"

ska::ScriptRunnerSystem::ScriptRunnerSystem(EntityManager& entityManager, GameEventDispatcher& ged) : 
	System(entityManager),
	m_entityManager(entityManager),
	parser {reservedKeywords},
	typeBuilder {parser, typeCrosser },
	symbolsTypeUpdater {parser},
	typeChecker {parser, typeCrosser },
	interpreter{reservedKeywords, typeCrosser },
	moduleConfiguration{scriptCache, typeBuilder, symbolsTypeUpdater, reservedKeywords, parser, interpreter} {
	
	ska::ScriptNameStandardLibraryPath() = "" SKALANG_DIR "/std/src/std/scripts/";

	//TODO : fill m_parameters
	m_modules.push_back(std::make_unique<lang::ParameterModule>(moduleConfiguration, m_parameters));
}

void ska::ScriptRunnerSystem::registerScript(const ScriptSleepComponent& scriptData, const EntityId triggerer, const std::optional<EntityId> target) {	
	auto executor = std::make_unique<ska::Script>( moduleConfiguration.scriptCache, "main", 
		ska::Tokenizer{ moduleConfiguration.reservedKeywords,
		"var Script = import \"scripts/" + scriptData.name + "\";"
		"var ParametersGenerator = import \"bind:std.native.parameter\";"
		"Script.run(ParametersGenerator.Gen(\"" + scriptData.name + "\"));"
		}.tokenize());


	executor->parse(parser);
	
	auto sc = ScriptComponent{ scriptData, triggerer, target.value_or(triggerer), std::move(executor) };


	m_scriptsToAddQueue.push_back(std::move(sc));
}

void ska::ScriptRunnerSystem::refresh(unsigned int) {
	for (auto& c : m_scriptsToAddQueue) {
		auto entity = m_entityManager.createEntity();
		m_componentAccessor.add<ScriptComponent>(entity, std::move(c));
	}
	m_scriptsToAddQueue.clear();

	auto nextScript = getHighestPriorityScript();
	if (nextScript == nullptr) {
		return;
	}

	try {
		nextScript->play(interpreter);
	} catch (ScriptDiedException& sde) {
		//Died. Nothing to do
	} catch (ScriptException e) {
		SLOG(LogLevel::Error) << "ERREUR SCRIPT [" << nextScript->name() << "] " << e.what();
	}

}

ska::ScriptComponent* ska::ScriptRunnerSystem::getHighestPriorityScript() {
	float maxPriorityScriptValue = -1;
	ScriptComponent* maxPriorityScript = nullptr;
	const auto currentTimeTicks = TimeUtils::getTicks();

	const auto& processed = getEntities();
	for (const auto& entityId : processed) {
		auto& script = m_componentAccessor.get<ScriptComponent>(entityId);
		const auto currentVal = script.getPriority(currentTimeTicks);
		if (maxPriorityScriptValue < currentVal) {
			maxPriorityScriptValue = currentVal;
			maxPriorityScript = &script;
		}
	}

	/* maxPriorityScriptValue < 0 means no script in an "OK state" found */
	return maxPriorityScriptValue > 0 ? maxPriorityScript : NULL;
}
