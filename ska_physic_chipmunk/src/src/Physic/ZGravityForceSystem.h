#pragma once
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
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
