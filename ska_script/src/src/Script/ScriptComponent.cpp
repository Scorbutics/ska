#include "Script/__internalConfig/LoggerConfig.h"
#include "ScriptComponent.h"
#include "Core/Utils/TimeUtils.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/Exceptions/ScriptDiedException.h"
#include "Interpreter/Interpreter.h"

SKA_DEFINE_COMPONENT(ska::ScriptComponent);

float ska::ScriptComponent::getPriority(const unsigned int currentTimeMillis) const {
  /* If the script cannot be played, we have a negative priority */
  if (!canBePlayed() || scriptPeriod == 0) {
    return -1;
  }

  /* In other ways this coeff variable make the priority calculation */
  const auto priorityFromElapsedTime = (static_cast<float>(currentTimeMillis) - lastTimeStarted) / scriptPeriod;

  if (state != ScriptState::PAUSED) {
    return priorityFromElapsedTime;
  } 

  /* A PAUSED script must gain rapidly high priority contrary to a STOPPED one.
  Then we use the exponential to simulate that */
  return NumberUtils::exponential(priorityFromElapsedTime);
}

bool ska::ScriptComponent::canBePlayed() const {
  switch(state) {
    case ScriptState::RUNNING:
    case ScriptState::DEAD:
      return false;
    
    case ScriptState::STOPPED:
      return triggeringType == ScriptTriggerType::AUTO;

    default:
      return (TimeUtils::getTicks() - lastTimeDelayed) <= delay;
  }
}

ska::ScriptState ska::ScriptComponent::updateFromCurrentTime() {
	/* If the script has been paused (m_active > 1), update the script status to PAUSED.
	If state is PAUSED and delay is past or if state is STOPPED, runs the script */
	if (state == ScriptState::PAUSED 
		&& TimeUtils::getTicks() - lastTimeDelayed  > delay) {
		state = ScriptState::RUNNING;
		delay = 0;
	} else {
		state = ScriptState::RUNNING;
	}
	return state;
}

bool ska::ScriptComponent::play(Interpreter& interpreter) {

	/* Update status once */
	if (updateFromCurrentTime() == ScriptState::PAUSED) {
		return false;
	}

	lastTimeStarted = TimeUtils::getTicks();
	try {	
		interpreter.script(*controller);
	} catch(std::exception& e) {
    kill();
		throw ScriptDiedException(e.what());
	}


	if ( /* script.controller->eof() */ true) {
		state = ScriptState::STOPPED;
		/* If the script is terminated and triggering is not automatic, then we don't reload the script */
		if (triggeringType == ScriptTriggerType::AUTO) {
			//TODO is this below "auto reload" ?
			state = ScriptState::PAUSED;
			lastTimeDelayed = TimeUtils::getTicks();
		}
	}
  return true;
}
