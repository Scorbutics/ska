#undef main
#include <iostream>
#include <SDL.h>
#include "LoggerConfig.h"
#include "BenchmarkerCore.h"
#include "Logging/Logger.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/Exceptions/StateDiedException.h"
#include "Particle/Impl/BoxParticleGenerator.h"
#include "Particle/Impl/ColorParticleUpdater.h"
#include "Particle/Impl/TimeParticleUpdater.h"
#include "Particle/Impl/PhysicParticleUpdater.h"
#include "Particle/Impl/ColoredRectGraphicParticleRenderer.h"
#include "Particle/Impl/BasicVelocityGenerator.h"
#include "Particle/Impl/BasicColorGenerator.h"
#include "Particle/Impl/ConsoleParticleCountRenderer.h"
#include "Particle/Impl/EulerAttractorParticleUpdater.h"
#include "Particle/Impl/SpreadingColorParticleEffectFactory.h"
#include "Particle/Impl/SpreadingParticleEffectFactory.h"
#include "Core/Data/Events/ExtensibleGameEventDispatcher.h"
#include "Particle/Impl/SpreadingTextureParticleEffectFactory.h"
#include "Base/IO/Files/FileUtils.h"
#include "Particle/Impl/SideBalancingParticleUpdater.h"
#include "CoreModule.h"
#include "GraphicModule.h"
#include "Core/Data/Events/ExtensibleGameEventDispatcher.h"
#include "Core/ECS/EntityManager.h"
#include "Core/Draw/VectorDrawableContainer.h"

BenchmarkerCore::BenchmarkerCore(GameConf&& gc) :
	m_window("ska Particle Benchmark", 1500, 900),
	m_particles(400, 70000),
	m_renderer(m_window, 0, SDL_RENDERER_ACCELERATED) {
	m_renderer.setRenderColor(ska::Color(0, 0, 0, 255));
	{
		ska::Point<int> origin(950, 500);
		ska::Point<int> maxDistance(300, 0);
		m_particles.addGenerator<ska::BoxParticleGenerator>(origin, maxDistance);
		ska::Color cEnd(110, 10, 140, 255);
		ska::Color cStart(120, 220, 150, 255);
		m_particles.addGenerator<ska::BasicColorGenerator>(cStart, cEnd);
		ska::PolarPoint<float> initialVelocity(00.F, 1.57F);
		m_particles.addGenerator<ska::BasicVelocityGenerator>(initialVelocity, 0, 1);

		ska::PolarPoint<float> force;
		force.radius = 200.F;
		m_attractor = &m_particles.addUpdater<ska::EulerAttractorParticleUpdater>(origin, force);
		m_particles.addUpdater<ska::PhysicParticleUpdater>();
		static const auto lifetime = 20000;
		m_particles.addUpdater<ska::ColorParticleUpdater>(lifetime);
		m_particles.addUpdater<ska::TimeParticleUpdater>(lifetime);

		m_particles.addRenderer<ska::ColoredRectGraphicParticleRenderer>(m_renderer, 1);
		m_particles.addRenderer<ska::ConsoleParticleCountRenderer>(500);

	}
	/**********************************************************/

	/*ska::SpreadingParticleSystemData effectData;
	effectData.lifetime = 2000;
	effectData.originalBoxSize.x = 15;
	effectData.origin.x = 750;
	effectData.origin.y = 450;
	effectData.initialVelocity.radius = 0.5F;
	effectData.initialVelocity.angle = 1.57F;
	effectData.spreading = 0.8F;
	effectData.density = 1;
	effectData.generationDelay = 350;
	effectData.maxParticles = 20;
	effectData.spritePath = ska::FileUtils::getExecutablePath() + "Sprites/Particles/4.png";
	auto& grassEffect = m_particleSystem.makeEffect<ska::SpreadingTextureParticleEffectFactory>(m_window.getRenderer(), effectData);
	grassEffect.addUpdater<ska::SideBalancingParticleUpdater>(effectData.origin, 1.F, 1.F);*/

	//buildFireworks();

}

void BenchmarkerCore::buildFireworks() {
	ska::Color cEnd(110, 30, 140, 255);
	ska::Color cStart(120, 220, 150, 255);
	ska::SpreadingParticleSystemData effectData;
	effectData.lifetime = 1500;
	effectData.origin.x = 750;
	effectData.origin.y = 450;
	effectData.cStart = cStart;
	effectData.cEnd = cEnd;
	effectData.initialVelocity.radius = 7.5F;
	effectData.initialVelocity.angle = 0.27F;
	effectData.spreading = M_PI;
	effectData.spreadingSlices = 10;
	effectData.density = 5;
	effectData.maxParticles = 50;
	m_particleSystem.makeEffect<ska::SpreadingColorParticleEffectFactory>(m_renderer, effectData);

	effectData.lifetime = 2000;
	effectData.cEnd = ska::Color(110, 30, 40, 255);
	effectData.cStart = ska::Color(220, 130, 150, 255);
	effectData.initialVelocity.radius = 4.F;
	effectData.origin.x = 950;
	effectData.origin.y = 650;
	m_particleSystem.makeEffect<ska::SpreadingColorParticleEffectFactory>(m_renderer, effectData);

	effectData.lifetime = 3500;
	effectData.cEnd = ska::Color(20, 30, 140, 255);
	effectData.cStart = ska::Color(150, 120, 220, 255);
	effectData.initialVelocity.radius = 3.F;
	effectData.origin.x = 1050;
	effectData.origin.y = 350;
	m_particleSystem.makeEffect<ska::SpreadingColorParticleEffectFactory>(m_renderer, effectData);

	effectData.lifetime = 2700;
	effectData.cEnd = ska::Color(120, 30, 70, 255);
	effectData.cStart = ska::Color(250, 120, 120, 255);
	effectData.initialVelocity.radius = 5.F;
	effectData.origin.x = 200;
	effectData.origin.y = 250;
	m_particleSystem.makeEffect<ska::SpreadingColorParticleEffectFactory>(m_renderer, effectData);
}

int BenchmarkerCore::onTerminate(ska::TerminateProcessException&) {
	return 0;
}

int BenchmarkerCore::onException(ska::GenericException& e) {
	std::cerr << "Uncaught exception : " << e.what() << std::endl;
	return 1;
}

void BenchmarkerCore::eventUpdate(const float ti) {
	m_inputListener.update();
	m_attractor->move(m_inputListener.getMouseInput().getMousePos());
	m_particles.refresh(static_cast<unsigned>(ti));
	//m_particleSystem.refresh(static_cast<unsigned>(ti));
}

void BenchmarkerCore::graphicUpdate(const unsigned long) const {
	static const ska::Color black(0,0,0,255);
	
	m_particles.render(m_renderer);
	//m_particleSystem.render(m_renderer);
	//m_fpsCalculator.getRenderable().render(m_renderer);
	m_renderer.update();
	
}

void BenchmarkerCore::run() {
	unsigned long t0 = ska::TimeUtils::getTicks();
	const auto ti = ticksWanted();
	auto accumulator = ti;

	for (;;) {
		const unsigned long t = ska::TimeUtils::getTicks();

		const auto ellapsedTime = t - t0;
		t0 = t;

		auto invAccu = 0U;

		accumulator += ellapsedTime;

		while (accumulator >= ti) {
			eventUpdate(ti);
			invAccu += static_cast<unsigned int>(ti);
			m_fpsCalculator.calculate(invAccu);
			accumulator -= ti;
		}

		graphicUpdate(ellapsedTime);
	}
}

float BenchmarkerCore::ticksWanted() const{
	static const unsigned int FPS = 60;
	static const float TICKS = 1000.F / FPS;
	return TICKS;
}

#undef main
int main(int argc, char* argv[]) {
	using GameConf = ska::GameConfiguration<
		ska::ExtensibleGameEventDispatcher<>, 
		ska::CoreModule<ska::EntityManager>, 
		ska::GraphicModule>;
	GameConf gc;
	auto& core = gc.requireModule<ska::CoreModule<ska::EntityManager>>("Core", ska::EntityManager::Make<>(gc.getEventDispatcher()), gc.getEventDispatcher());
	auto window = std::make_unique<ska::SDLWindow>("Particle benchmark", 1500, 900);
	auto renderer = std::make_unique<ska::SDLRenderer>(*window, 0, SDL_RENDERER_ACCELERATED);
	auto dc = std::make_unique<ska::VectorDrawableContainer>(*renderer);

	gc.requireModule<ska::GraphicModule>("Graphics", gc.getEventDispatcher(), std::move(dc), std::move(renderer), std::move(window));
	auto app = std::make_unique<BenchmarkerCore>(std::move(gc));
	app->run();
	return 0;
}
