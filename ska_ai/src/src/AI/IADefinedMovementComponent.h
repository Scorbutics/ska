#pragma once
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Core/Point.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/ECS/Component.h"
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

SKA_DECLARE_COMPONENT(ska::IADefinedMovementComponent);
