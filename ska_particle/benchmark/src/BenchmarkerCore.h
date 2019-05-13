#pragma once
#include "Core/GameApp.h"
#include "Particle/ParticleEffect.h"
#include "Core/Inputs/RawInputListener.h"
#include "Graphic/TicksCalculator.h"
#include "Particle/Impl/EulerAttractorParticleUpdater.h"
#include "Particle/ParticleSystem.h"
#include "Graphic/SDLRenderer.h"
#include "Graphic/SDLWindow.h"
#include "Core/Data/Events/ExtensibleGameEventDispatcher.h"
#include "GraphicModule.h"
#include "Core/ECS/EntityManager.h"
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

