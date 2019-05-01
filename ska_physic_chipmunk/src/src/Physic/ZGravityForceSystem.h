#pragma once
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "Body.h"

namespace ska {
	class Space;

	class ZGravityForceSystem :
		public System<RequiredComponent<ForceComponent, PositionComponent, HitboxComponent, MovementComponent>, PossibleComponent<>> {
	public:
		explicit ZGravityForceSystem(ska::EntityManager& em, float gravityPower);
		~ZGravityForceSystem() override = default;
		
	protected:
		void refresh(unsigned int ellapsedTime) override;
	
	private:
		const float m_gravityPower;
	};	
}
