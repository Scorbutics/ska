#pragma once
#include "ECS/SerializableComponent.h"
#include "Point.h"

namespace ska {
	namespace RandomMovementType {
		enum Enum {
			NO_GOAL,
			CIRCLE_AROUND
		};
	}

	class IARandomMovementComponent : 
		public SerializableComponent {
	public:
		IARandomMovementComponent() {
			lastTimeStarted = 0;
			delay = 0;
			direction = 0;
			type = RandomMovementType::NO_GOAL;
		}

		char direction;
		char type;
		EntityId emitter;
		unsigned int delay;
		unsigned int lastTimeStarted;
	};
}
