#pragma once
#include "EventDispatcher.h"
#include "SoundEvent.h"
#include "CollisionEvent.h"
#include "WorldEvent.h"
#include "GameEvent.h"
#include "GUIEvent.h"
#include "StateEvent.h"
#include "InputKeyEvent.h"
#include "InputMouseEvent.h"
#include "ECSEvent.h"
#include "ScriptEvent.h"

namespace ska {
	/**
	* \brief Defines a basic list of common events in a game.
	*/
	using GameEventDispatcher = EventDispatcher<SoundEvent, WorldEvent, CollisionEvent, GUIEvent, GameEvent, StateEvent, InputKeyEvent, InputMouseEvent, ECSEvent, ScriptEvent>;
}
