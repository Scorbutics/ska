#pragma once
#include "GameApp.h"
#include "ParticleEffect.h"
#include "Inputs/RawInputListener.h"
#include "TicksCalculator.h"
#include "Impl/EulerAttractorParticleUpdater.h"
#include "ParticleSystem.h"
#include "Graphic/SDLRenderer.h"
#include "Graphic/SDLWindow.h"
#include "Data/Events/ExtensibleGameEventDispatcher.h"
#include "GraphicModule.h"
#include "ECS/EntityManager.h"
#include "CoreModule.h"

class BenchmarkerCore : 
	public ska::GameApp {
	using GameConf = ska::GameConfiguration<ska::ExtensibleGameEventDispatcher<>, ska::CoreModule<ska::EntityManager>, ska::GraphicModule>;
public:
	explicit BenchmarkerCore(GameConf&& gc);
	virtual ~BenchmarkerCore() = default;
	int onTerminate(ska::TerminateProcessException&) override;
	int onException(ska::GenericException&) override;
	void eventUpdate(const float ti);
	void graphicUpdate(const unsigned long ellapsed_time) const;
	void run() override;
	float ticksWanted() const override;

private:
	void buildFireworks();

	ska::RawInputListener m_inputListener;
	ska::SDLWindow m_window;
	ska::ParticleEffect m_particles;
	ska::ParticleSystem m_particleSystem;
	ska::EulerAttractorParticleUpdater* m_attractor;
	ska::TicksCalculator m_fpsCalculator;
	ska::SDLRenderer m_renderer;
};

