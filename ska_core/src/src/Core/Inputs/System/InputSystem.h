#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../../ECS/Basics/Input/InputComponent.h"
#include "../../ECS/Basics/Physic/ForceComponent.h"
#include "../../ECS/Basics/Physic/PositionComponent.h"
#include "../InputContextManager.h"
#include "Base/Patterns/SubObserver.h"

namespace ska {
	struct InputKeyEvent;

	class InputSystem : 
		public System<RequiredComponent<PositionComponent, InputComponent, ForceComponent>, PossibleComponent<>>,
		public SubObserver<InputKeyEvent> {
	public:
		InputSystem(EntityManager& entityManager, GameEventDispatcher& ged);
		InputSystem& operator=(const InputSystem&) = delete;
		~InputSystem() override = default;
	
	private:
		bool onKeyInputEvent(InputKeyEvent & ike);

	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	};
}
