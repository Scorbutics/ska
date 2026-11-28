#pragma once

#include <bitset>

namespace ska {
	enum InputAction {
		Jump,
		DoAction,
		ShotSkill1,
		ShotSkill2,
		ShotSkill3,
		ShotSkill4,
		Quit,
		UseObject1,
		UseObject2,
		UseObject3,
		UseObject4,
		UseObject5,
		UseObject6,
		PathfindTo,
		LClic,
		RClic,
		//GUI
		DeleteChar,
		// An item chosen in a slot (ex : the item of the trainer in a battle) : used, the next one chosen
		UseItem,
		NextItem,
		// A special move (ex : the attack of the bond between a trainer and its Pokemon in a battle)
		Special,
		// The menu of the game (opened, closed) ; back, cancel (in a menu)
		Menu,
		Cancel,
		// The tools of the developers (ex : the debug panel)
		Debug
	};

#define INPUT_MAX_ACTION 23

	typedef std::bitset<INPUT_MAX_ACTION> InputActionContainer;
}
