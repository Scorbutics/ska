#include "Script/__internalConfig/LoggerConfig.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "ScriptComponent.h"
#include "Core/Utils/TimeUtils.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/Exceptions/ScriptDiedException.h"
#include "Interpreter/Interpreter.h"

SKA_DEFINE_COMPONENT(ska::ScriptComponent);

ska::ScriptComponent::ScriptComponent(const ScriptSleepComponent& scriptData, EntityId triggerer, EntityId target, std::unique_ptr<Script> instance) :
  m_controller(std::move(instance)) {
	m_name = scriptData.name;
	m_origin = triggerer;
	m_target = target;
	m_scriptPeriod = scriptData.period == 0 ? 1 : scriptData.period;
	m_extraArgs = scriptData.args;
	m_triggeringType = ScriptTriggerType::AUTO;
	m_deleteEntityWhenFinished = scriptData.deleteEntityWhenFinished;
}

float ska::ScriptComponent::getPriority(const unsigned int currentTimeMillis) const {
  /* If the script cannot be played, we have a negative priority */
  if (!canBePlayed() || m_scriptPeriod == 0) {
    return -1;
  }

  /* In other ways this coeff variable make the priority calculation */
  const auto priorityFromElapsedTime = (static_cast<float>(currentTimeMillis) - m_lastTimeStarted) / m_scriptPeriod;

  if (m_state != ScriptState::PAUSED) {
    return priorityFromElapsedTime;
  } 

  /* A PAUSED script must gain rapidly high priority contrary to a STOPPED one.
  Then we use the exponential to simulate that */
  return NumberUtils::exponential(priorityFromElapsedTime);
}

bool ska::ScriptComponent::canBePlayed() const {
  switch(m_state) {
    case ScriptState::RUNNING:
    case ScriptState::DEAD:
      return false;
    
    case ScriptState::STOPPED:
      return m_triggeringType == ScriptTriggerType::AUTO;

    default:
      return (TimeUtils::getTicks() - m_lastTimeDelayed) <= m_delay;
  }
}

ska::ScriptState ska::ScriptComponent::updateFromCurrentTime() {
	/* If the script has been paused (m_active > 1), update the script status to PAUSED.
	If state is PAUSED and delay is past or if state is STOPPED, runs the script */
	if (m_state == ScriptState::PAUSED 
		&& TimeUtils::getTicks() - m_lastTimeDelayed  > m_delay) {
		m_state = ScriptState::RUNNING;
		m_delay = 0;
	} else {
		m_state = ScriptState::RUNNING;
	}
	return m_state;
}

bool ska::ScriptComponent::play(Interpreter& interpreter) {

	/* Update status once */
	if (updateFromCurrentTime() == ScriptState::PAUSED) {
		return false;
	}

	m_lastTimeStarted = TimeUtils::getTicks();
	try {	
		interpreter.script(*m_controller);
	} catch(std::exception& e) {
    kill();
		throw ScriptDiedException(e.what());
	}


	if ( /* script.controller->eof() */ true) {
		m_state = ScriptState::STOPPED;
		/* If the script is terminated and triggering is not automatic, then we don't reload the script */
		if (m_triggeringType == ScriptTriggerType::AUTO) {
			//TODO is this below "auto reload" ?
			m_state = ScriptState::PAUSED;
			m_lastTimeDelayed = TimeUtils::getTicks();
		}
	}
  return true;
}
