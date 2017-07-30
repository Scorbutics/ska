#pragma once
#include "Core/GameCore.h"
#include "ECS/EntityManager.h"
#include "Data/Events/GameEventDispatcher.h"
#include "Draw/VectorDrawableContainer.h"
#include "Audio/SoundRenderer.h"
#include "Data/Events/ExtensibleGameEventDispatcher.h"

using GameBase = ska::GameCore<ska::EntityManager, ska::ExtensibleGameEventDispatcher<>, ska::VectorDrawableContainer, ska::SoundRenderer>;
class Game : 
	public GameBase {

public:
	Game() = default;
	void init();
	virtual ~Game() = default;

private:
	int onTerminate(ska::TerminateProcessException & tpe) override;
	int onException(ska::GenericException & e) override;
};
