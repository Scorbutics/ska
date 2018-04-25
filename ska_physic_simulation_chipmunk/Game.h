#pragma once
#include "Core/GameCore.h"
#include "ECS/EntityManager.h"
#include "Data/Events/ExtensibleGameEventDispatcher.h"
#include "CoreModule.h"
#include "GraphicModule.h"


using GameBase = ska::GameCore<ska::ExtensibleGameEventDispatcher<>, ska::CoreModule<ska::EntityManager>, ska::GraphicModule>;
class Game :
	public GameBase {

public:
	Game(ska::EntityManager& em, GameConfPtr&& gc);
	virtual ~Game() = default;

private:
	int onTerminate(ska::TerminateProcessException & tpe) override;
	int onException(ska::GenericException & e) override;

	ska::EntityManager& m_entityManager;
};
