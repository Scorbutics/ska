#pragma once
#include "AnimationStateMachine.h"


namespace ska {
	class EntityManager;

	class DirectionnalAnimationStateMachine :
		public AnimationStateMachine<> {
	public:

		explicit DirectionnalAnimationStateMachine(EntityManager& em);
		virtual void update(AnimationComponent&, EntityId&) override;
	private:
		EntityManager& m_entityManager;
	};
}
