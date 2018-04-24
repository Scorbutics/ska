#pragma once
#include "Data/Events/ExtensibleGameEventDispatcher.h"
#include "ECS/EntityManager.h"
#include "Core/State/StateBase.h"
#include "Utils/SubObserver.h"
#include "TileWorld.h"
#include "Physic/System/EntityCollisionResponse.h"
#include "Graphic/System/WalkAnimationStateMachine.h"
#include "Graphic/DebugEntityCollisionResponse.h"
#include "Physic/System/WorldCollisionResponse.h"
#include "Physic/System/DebugWorldCollisionResponse.h"

class StateSandbox :
	public ska::StateBase,
	public ska::SubObserver<ska::GameEvent> {
public:
	StateSandbox(ska::EntityManager& em, ska::ExtensibleGameEventDispatcher<>&);

	virtual ~StateSandbox() = default;

	virtual void onGraphicUpdate(unsigned int, ska::DrawableContainer&) override;
	virtual void onEventUpdate(unsigned int) override;

private:
	bool onGameEvent(ska::GameEvent& ge);
	ska::EntityId createPhysicAABBEntity(ska::Point<int> pos, const std::string& sprite, bool spritesheet) const;

	ska::CameraSystem* m_cameraSystem;

	ska::TilesetCorrespondanceMapper m_mapper;
	ska::Tileset m_tileset;

	ska::ExtensibleGameEventDispatcher<>& m_eventDispatcher;
	ska::EntityManager& m_entityManager;

	ska::TileWorld m_world;

	ska::DebugEntityCollisionResponse m_debugEntityCollision;
	ska::EntityCollisionResponse m_entityCollision;
	ska::DebugWorldCollisionResponse m_debugWorldCollision;
	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::WalkAnimationStateMachine* m_walkASM;
};

