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
#include "Base/Values/Strings/StringUtils.h"
#include "Base/IO/Files/FileUtils.h"
#include "Core/Utils/TimeUtils.h"
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
	
	ScriptComponent sc;
	sc.active = 0;
	sc.name = scriptData.name;
	sc.origin = triggerer;
	sc.target = target.value_or(triggerer);
	sc.controller = std::move(executor);

	sc.scriptPeriod = scriptData.period == 0 ? 1 : scriptData.period;
	
	//TODO : fill m_parameters
	sc.extraArgs = scriptData.args;
	
	//sc.context = scriptData.context;
	sc.triggeringType = ScriptTriggerType::AUTO;
	sc.deleteEntityWhenFinished = scriptData.deleteEntityWhenFinished;

	m_componentToAddQueue.push_back(std::move(sc));
}

void ska::ScriptRunnerSystem::refresh(unsigned int) {
	for (auto& c : m_componentToAddQueue) {
		auto entity = m_entityManager.createEntity();
		m_componentAccessor.add<ScriptComponent>(entity, std::move(c));
	}
	m_componentToAddQueue.clear();

	auto nextScript = getHighestPriorityScript();
	if (nextScript == nullptr) {
		return;
	}

	try {
		play(*nextScript);
	} catch (ScriptDiedException& sde) {
		/*
		const std::string& entityScriptId = sde.getScript();
		if (entityScriptId.empty()) {
			*/
			kill(*nextScript);
		/*} else {
			if (StringUtils::isInt(entityScriptId, 10)) {
				EntityId scriptEntity = StringUtils::strToInt(entityScriptId);
				const auto& scriptCPtr = m_componentPossibleAccessor.get<ScriptComponent>(scriptEntity);
				if (scriptCPtr == nullptr) {
					SLOG(LogLevel::Error) << "ERREUR SCRIPT [" << nextScript->extendedName << "] " << sde.what() << " Script not found with id : " << entityScriptId;
				} else {
					kill(*scriptCPtr);
				}
			} else {
				SLOG(LogLevel::Error) << "ERREUR SCRIPT [" << nextScript->extendedName << "] " << sde.what() << " This is not an integer id : " << entityScriptId;
			}
		}*/

	} catch (ScriptException e) {
		SLOG(LogLevel::Error) << "ERREUR SCRIPT [" << nextScript->name << "] " << e.what();
	}

}

void ska::ScriptRunnerSystem::kill(ScriptComponent& script) const {
	//TODO suppression de scripts dans la save

	//string& tmpScritFileName = ("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + savegame.getSaveName() + FILE_SEPARATOR "tmpscripts.data");
	//std::ofstream scriptList;
	/*scriptList.open(tmpScritFileName.c_str(), ios::app);
	if (!scriptList.fail()) {
		scriptList << script.extendedName << endl;
		scriptList.close();
	}*/

	script.state = ScriptState::DEAD;
}

ska::ScriptComponent* ska::ScriptRunnerSystem::getHighestPriorityScript() {
	float maxPriorityScriptValue = -1;
	ScriptComponent* maxPriorityScript = nullptr;
	auto currentTimeTicks = TimeUtils::getTicks();

	const auto& processed = getEntities();
	for (const auto& entityId : processed) {
		auto& script = m_componentAccessor.get<ScriptComponent>(entityId);
		const auto currentVal = getPriority(script, currentTimeTicks);
		if (maxPriorityScriptValue < currentVal) {
			maxPriorityScriptValue = currentVal;
			maxPriorityScript = &script;
		}
	}

	/* maxPriorityScriptValue < 0 means no script in an "OK state" found */
	return maxPriorityScriptValue > 0 ? maxPriorityScript : NULL;
}

bool ska::ScriptRunnerSystem::canBePlayed(ScriptComponent& script) {
	transferActiveToDelay(script);
	const auto cannotBePlayed =
		ScriptState::RUNNING == script.state || ScriptState::DEAD == script.state
		|| script.scriptPeriod == 0
		|| script.active > 0
		|| (TimeUtils::getTicks() - script.lastTimeDelayed) <= script.delay
		|| !(script.triggeringType == ScriptTriggerType::AUTO && script.state == ScriptState::STOPPED || script.state != ScriptState::STOPPED);

	return !cannotBePlayed;
}


/* When possible, transfers the value of m_active containing a time to wait to m_delay */
bool ska::ScriptRunnerSystem::transferActiveToDelay(ScriptComponent& script) {
	if (script.active > 1) {
		script.state = ScriptState::PAUSED;
		script.delay = script.active;
		script.lastTimeDelayed = TimeUtils::getTicks();
		script.active = 0;
		return true;
	}
	return false;
}

bool ska::ScriptRunnerSystem::forcePlay(ScriptComponent& script) {

	script.lastTimeStarted = TimeUtils::getTicks();

	/* Update status once */
	if (manageCurrentState(script) == ScriptState::PAUSED) {
		return false;
	}

	try {	
		interpreter.script(*script.controller);
	} catch(std::exception& e) {
		throw ScriptDiedException(e.what());
	}


	if ( /* script.controller->eof() */ false) {
		script.state = ScriptState::STOPPED;
		/* If the script is terminated and triggering is not automatic, then we don't reload the script */
		if (script.triggeringType == ScriptTriggerType::AUTO) {
			//TODO auto reload?
		}
	}

	return true;
}

float ska::ScriptRunnerSystem::getPriority(ScriptComponent& script, const unsigned int currentTimeMillis) {
	/* If the script cannot be played, we have a negative priority */
	if (!canBePlayed(script) || script.scriptPeriod == 0) {
		return -1;
	}

	/* In other ways this coeff variable make the priority calculation */
	const auto priorityFromElapsedTime = (static_cast<float>(currentTimeMillis) - script.lastTimeStarted) / script.scriptPeriod;

	if (script.state != ScriptState::PAUSED) {
		return priorityFromElapsedTime;
	} 

	/* A PAUSED script must gain rapidly high priority contrary to a STOPPED one.
	Then we use the exponential to simulate that */
	return NumberUtils::exponential(priorityFromElapsedTime);
}

ska::ScriptState ska::ScriptRunnerSystem::manageCurrentState(ScriptComponent& script) {
	/* If the script has been paused (m_active > 1), update the script status to PAUSED.
	If state is PAUSED and delay is past or if state is STOPPED, runs the script */
	if (transferActiveToDelay(script)) {
	} else if (script.state == ScriptState::PAUSED && TimeUtils::getTicks() - script.lastTimeDelayed  > script.delay) {
		script.state = ScriptState::RUNNING;
		script.delay = 0;
	} else {
		script.state = ScriptState::RUNNING;
	}
	return script.state;
}

void ska::ScriptRunnerSystem::stop(ScriptComponent& script) {
	/* kind of delete the script */
	script.state = ScriptState::STOPPED;
	script.triggeringType = ScriptTriggerType::NONE;
}

