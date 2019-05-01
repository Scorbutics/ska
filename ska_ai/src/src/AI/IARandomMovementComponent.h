#pragma once
#include "Point.h"
#include "../../ska_draw/Graphic/DebugEntityCollisionResponse.h"

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
