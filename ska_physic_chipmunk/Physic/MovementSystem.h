#pragma once
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/System.h"
#include "BodyComponent.h"

namespace ska {
	namespace cp {
		class MovementSystem :
			public System<std::unordered_set<EntityId>, RequiredComponent<ForceComponent, BodyComponent>, PossibleComponent<>> {
		public:
			explicit MovementSystem(ska::EntityManager& em);
			~MovementSystem() override = default;
		
		protected:
			virtual void refresh(unsigned int ellapsedTime) override;
		};
	}
}
