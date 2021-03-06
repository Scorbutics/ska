#pragma once
#include "Core/Point.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/ECS/Component.h"
namespace ska {
	namespace RandomMovementType {
		enum Enum {
			NO_GOAL,
			CIRCLE_AROUND
		};
	}

	class IARandomMovementComponent {
	public:
		IARandomMovementComponent() {
			lastTimeStarted = 0;
			delay = 0;
			direction = 0;
			type = RandomMovementType::NO_GOAL;
		}

		char direction;
		char type;
		ska::EntityId emitter;
		unsigned int delay;
		unsigned int lastTimeStarted;
	};
}

SKA_DECLARE_COMPONENT(ska::IARandomMovementComponent);
