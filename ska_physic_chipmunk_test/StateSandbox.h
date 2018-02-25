#pragma once
#include "Data/Events/ExtensibleGameEventDispatcher.h"
#include "ECS/EntityManager.h"
#include "Core/State/StateBase.h"
#include "Utils/SubObserver.h"
#include "Polygon.h"

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

	ska::CameraSystem* m_cameraSystem;
	
	ska::ExtensibleGameEventDispatcher<>& m_eventDispatcher;
	ska::EntityManager& m_entityManager;
	Polygon<int> m_layerContour;
};

