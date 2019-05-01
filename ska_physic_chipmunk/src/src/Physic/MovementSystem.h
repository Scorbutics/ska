#pragma once
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "Body.h"

namespace ska {
	namespace cp {
		class Space;

		class MovementSystem :
			public System< RequiredComponent<ForceComponent, PositionComponent, HitboxComponent, MovementComponent>, PossibleComponent<>> {
		public:
			explicit MovementSystem(ska::EntityManager& em, Space& space);
			~MovementSystem() override = default;
		
		protected:
			void refresh(unsigned int ellapsedTime) override;

		private:
			void adjustVelocity(const ska::EntityId& entityId, ska::cp::Body& body, const ska::ForceComponent& fc);
			Space& m_space;
		};
	}
}
