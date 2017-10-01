#pragma once
#include "Data/Events/ExtensibleGameEventDispatcher.h"
#include "ECS/EntityManager.h"
#include "Core/State/StateBase.h"
#include "Utils/SubObserver.h"
#include "World.h"
#include "Physic/System/EntityCollisionResponse.h"
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/WalkAnimationStateMachine.h"

class StateSandbox :
	public ska::StateBase<ska::EntityManager, ska::ExtensibleGameEventDispatcher<>>,
	public ska::SubObserver<ska::GameEvent> {
public:
	StateSandbox(StateData& data, ska::StateHolder& sh);
	
	virtual ~StateSandbox() = default;
	
	virtual void onGraphicUpdate(unsigned int, ska::DrawableContainer&) override;
	virtual void onEventUpdate(unsigned int) override;

private:
	bool onGameEvent(ska::GameEvent& ge);
	ska::EntityId createPhysicAABBEntity(ska::Point<int> pos, const std::string& sprite, bool spritesheet) const;

	ska::CameraSystem* m_cameraSystem;
	World m_world;
	ska::ExtensibleGameEventDispatcher<>& m_eventDispatcher;
	ska::EntityManager& m_entityManager;
	ska::EntityCollisionResponse m_entityCollision;
	ska::WalkAnimationStateMachine* m_walkASM;
};

