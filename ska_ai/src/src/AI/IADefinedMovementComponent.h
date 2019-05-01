#pragma once
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Point.h"
#include "Utils/TimeUtils.h"

namespace ska {
	class IADefinedMovementComponent {
	public:
		IADefinedMovementComponent(): 
			speed(12.F) {
			delay = 0;
			directionIndex = 0;
			loop = false;
			callbackActive = false;
			lastTimeStarted = TimeUtils::getTicks();
		}

		bool loop;
		unsigned int directionIndex;
		std::vector<Point<int>> directions;
		unsigned int delay;
		Point<int> origin;
		unsigned int lastTimeStarted;

		bool callbackActive;
		ScriptSleepComponent callback;
		float speed;
	};
}