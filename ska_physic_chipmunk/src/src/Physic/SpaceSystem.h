#pragma once
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/System.h"
#include "Space.h"

namespace ska {
	namespace cp {
		class SpaceSystem :
			public System<RequiredComponent<PositionComponent, HitboxComponent, MovementComponent, ForceComponent>, PossibleComponent<>> {
		public:
			SpaceSystem(EntityManager& em);
			Space& getSpace();
			void reset();

		protected:
			void refresh(unsigned int ellapsedTime) override;

		private:
			Space m_space;
			static constexpr auto FLUID_Z_FRICTION_FORCE = 0.12F;
		};
	}
}