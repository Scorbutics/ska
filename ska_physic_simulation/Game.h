#pragma once
#include "Core/GameCore.h"
#include "ECS/EntityManager.h"
#include "Draw/VectorDrawableContainer.h"
#include "Audio/SoundRenderer.h"
#include "Data/Events/ExtensibleGameEventDispatcher.h"

using GameBase = ska::GameCore<ska::ExtensibleGameEventDispatcher<>>;
class Game :
	public GameBase {

public:
	Game(ska::EntityManager& em, GameConf&& gc);
	virtual ~Game() = default;

private:
	int onTerminate(ska::TerminateProcessException & tpe) override;
	int onException(ska::GenericException & e) override;

	ska::EntityManager& m_entityManager;
};
