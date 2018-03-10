#pragma once
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/System.h"
#include "BodyComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"

namespace ska {
	namespace cp {
		class Space;

		class MovementSystem :
			public System<std::unordered_set<EntityId>, RequiredComponent<ForceComponent, PositionComponent, BodyComponent>, PossibleComponent<>> {
		public:
			explicit MovementSystem(ska::EntityManager& em, Space& space);
			~MovementSystem() override = default;
		
		protected:
			virtual void refresh(unsigned int ellapsedTime) override;

		private:
			Space& m_space;
		};
	}
}
