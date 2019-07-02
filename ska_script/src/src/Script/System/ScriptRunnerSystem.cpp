#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "Script/__internalConfig/LoggerConfig.h"
#include "ScriptRunnerSystem.h"
#include "Core/Exceptions/ScriptDiedException.h"
#include "Core/Exceptions/ScriptUnknownCommandException.h"
#include "Core/Exceptions/IllegalArgumentException.h"
#include "../ScriptUtils.h"
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

//Par d�faut, un script "permanent" se rafra�chit toutes les 1 ms
#define SCRIPT_DEFAULT_PERIOD 1
#define MAX_CONSECUTIVE_COMMANDS_PLAYED 5


ska::ScriptRunnerSystem::ScriptRunnerSystem(EntityManager& entityManager, GameEventDispatcher& ged) : 
	System(entityManager),
	m_entityManager(entityManager),
	parser {reservedKeywords},
	typeBuilder {parser, typeCrosser },
	symbolsTypeUpdater {parser},
	typeChecker {parser, typeCrosser },
	interpreter{reservedKeywords, typeCrosser },
	moduleConfiguration{scriptCache, typeBuilder, symbolsTypeUpdater, reservedKeywords, parser, interpreter} {
	//sch.setupCommands(m_commands);
	
	ska::ScriptNameStandardLibraryPath() = "" SKALANG_DIR "/std/src/std/scripts/";

	//TODO : fill args
	//TODO clean up that statics
	static auto parameterValues = std::vector<ska::NodeValue>{};
	static auto parameterModule = ska::lang::ParameterModule(moduleConfiguration, parameterValues);
}

std::string ska::ScriptRunnerSystem::map(const std::string& key, const EntityId id) const {
	return "";
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
	sc.parent = this;
	sc.fullPath = scriptData.name;
	sc.key = "";
	sc.origin = triggerer;
	sc.target = target.value_or(triggerer);
	sc.controller = std::move(executor);

	sc.extendedName = scriptData.name;
	sc.scriptPeriod = scriptData.period == 0 ? 1 : scriptData.period;
	sc.extraArgs = scriptData.args;
	sc.context = scriptData.context;
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
		const std::string& entityScriptId = sde.getScript();
		if (entityScriptId.empty()) {
			killAndSave(*nextScript);
		} else {
			if (StringUtils::isInt(entityScriptId, 10)) {
				EntityId scriptEntity = StringUtils::strToInt(entityScriptId);
				const auto& scriptCPtr = m_componentPossibleAccessor.get<ScriptComponent>(scriptEntity);
				if (scriptCPtr == nullptr) {
					SLOG(LogLevel::Error) << "ERREUR SCRIPT [" << nextScript->extendedName << "] " << sde.what() << " Script not found with id : " << entityScriptId;
				} else {
					killAndSave(*scriptCPtr);
				}
			} else {
				SLOG(LogLevel::Error) << "ERREUR SCRIPT [" << nextScript->extendedName << "] " << sde.what() << " This is not an integer id : " << entityScriptId;
			}
		}

	} catch (ScriptException e) {
		SLOG(LogLevel::Error) << "ERREUR SCRIPT [" << nextScript->extendedName << "] " << e.what();
	}

}

void ska::ScriptRunnerSystem::killAndSave(ScriptComponent& script) const {
	//TODO suppression de scripts dans la save

	//string& tmpScritFileName = ("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + savegame.getSaveName() + FILE_SEPARATOR "tmpscripts.data");
	//std::ofstream scriptList;
	/*scriptList.open(tmpScritFileName.c_str(), ios::app);
	if (!scriptList.fail()) {
		scriptList << script.extendedName << endl;
		scriptList.close();
	}*/

	script.state = EnumScriptState::DEAD;
}

ska::ScriptComponent* ska::ScriptRunnerSystem::getHighestPriorityScript() {
	float maxPriorityScriptValue = -1;
	ScriptComponent* maxPriorityScript = nullptr;
	auto currentTimeTicks = TimeUtils::getTicks();

	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& script = m_componentAccessor.get<ScriptComponent>(entityId);
		auto currentVal = getPriority(script, currentTimeTicks);
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
	auto cannotBePlayed =
		EnumScriptState::RUNNING == script.state
		|| EnumScriptState::DEAD == script.state
		|| script.active > 0
		|| (TimeUtils::getTicks() - script.lastTimeDelayed) <= script.delay
		|| !((script.triggeringType == ScriptTriggerType::AUTO && script.state == EnumScriptState::STOPPED) || (script.state != EnumScriptState::STOPPED));

	return !cannotBePlayed;
}


/* When possible, transfers the value of m_active containing a time to wait to m_delay */
bool ska::ScriptRunnerSystem::transferActiveToDelay(ScriptComponent& script) {
	if (script.active > 1) {
		script.state = EnumScriptState::PAUSED;
		script.delay = script.active;
		script.lastTimeDelayed = TimeUtils::getTicks();
		script.active = 0;
		return true;
	}
	return false;
}

bool ska::ScriptRunnerSystem::play(ScriptComponent& script) {

	if (!canBePlayed(script)) {
		return false;
	}

	script.lastTimeStarted = TimeUtils::getTicks();

	/* Update status once */
	if (manageCurrentState(script) == EnumScriptState::PAUSED) {
		return false;
	}

	script.parent = this;

	try {	
		interpreter.script(*script.controller);
	} catch(std::exception& e) {
		throw ScriptDiedException(e.what());
	}

	/* 
	If loop is exited it means that it's terminated or the script is stopped/paused.
	That's why we rewind m_fscript.
	(with a paused script here, it means that the script will resume next time we play it)
	*/

	if ( /* script.controller->eof() */ false) {
		script.state = EnumScriptState::STOPPED;
		/* If the script is terminated and triggering is not automatic, then we don't reload the script */
		if (script.triggeringType == ScriptTriggerType::AUTO) {
			//TODO ?
		}
		script.commandsPlayed = 0;
	}

	return true;
}

std::string ska::ScriptRunnerSystem::interpret(ScriptComponent& script, const std::string& cmd) {
	return "";
}

float ska::ScriptRunnerSystem::getPriority(ScriptComponent& script, const unsigned int currentTimeMillis) {
	/* If the script cannot be played, we have a negative priority */
	if (!canBePlayed(script)) {
		return -1;
	}

	/* If the script is automatic (or still running or paused) and has a 0 script period, then it's max priority */
	if (script.scriptPeriod == 0) {
		return (std::numeric_limits<float>::max)();
	}

	/* In other ways this coeff variable make the priority calculation */
	const float coeff = ((float)currentTimeMillis - script.lastTimeStarted) / script.scriptPeriod;

	if (script.state != EnumScriptState::PAUSED) {
		return coeff;
	} else {
		/* A PAUSED script must gain rapidly high priority contrary to a STOPPED one.
		Then we use the exponential to simulate that */
		return NumberUtils::exponential(coeff);
	}
}

ska::ScriptState ska::ScriptRunnerSystem::manageCurrentState(ScriptComponent& script) {
	/* If too many commands have been played or if the script has been paused (m_active > 1),
	update the script status to PAUSED.
	If state is PAUSED and delay is passed or if state is STOPPED, runs the script */
	if (script.commandsPlayed == MAX_CONSECUTIVE_COMMANDS_PLAYED) {
		script.state = EnumScriptState::PAUSED;
		script.commandsPlayed = 0;
	} else if (transferActiveToDelay(script)) {
	} else if (script.state == EnumScriptState::PAUSED && TimeUtils::getTicks() - script.lastTimeDelayed  > script.delay) {
		script.state = EnumScriptState::RUNNING;
		script.delay = 0;
	} else {
		script.state = EnumScriptState::RUNNING;
	}
	return script.state;
}

void ska::ScriptRunnerSystem::stop(ScriptComponent& script) {
	/* kind of delete the script */
	script.state = EnumScriptState::STOPPED;
	script.triggeringType = ScriptTriggerType::NONE;
}

