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
	m_triggeringType = scriptData.triggeringType;
	m_deleteEntityWhenFinished = scriptData.deleteEntityWhenFinished;
    m_lastTimeDelayed = TimeUtils::getTicks();
    updateState();
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

  /* A PAUSED script must gain rapidly high priority contrary to a STOPPED one */
    return priorityFromElapsedTime * priorityFromElapsedTime;
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

ska::ScriptState ska::ScriptComponent::updateState() {
switch(m_state) {
case ScriptState::PAUSED:
    if(TimeUtils::getTicks() - m_lastTimeDelayed  > m_delay) {
        m_state = ScriptState::RUNNING;
        m_delay = 0;
    }
    break;

case ScriptState::STOPPED:
    if (m_triggeringType == ScriptTriggerType::AUTO) {
        //auto reload
        m_state = ScriptState::PAUSED;
        m_lastTimeDelayed = TimeUtils::getTicks();
    } 
    break;

default:
    m_state = ScriptState::RUNNING;
    break;
}

return m_state;
}

bool ska::ScriptComponent::play(Interpreter& interpreter) {
/* Update status once */
if (updateState() == ScriptState::PAUSED) {
    return false;
}

    m_lastTimeStarted = TimeUtils::getTicks();
    try {	
        interpreter.script(*m_controller);
        m_state = ScriptState::STOPPED;
    } catch(std::exception& e) {
        kill();
        throw ScriptDiedException(e.what());
    }
  return true;
}

