#include "StateSandbox.h"
#include "Graphic/System/GraphicSystem.h"
#include "Graphic/System/CameraFixedSystem.h"
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/WalkAnimationStateMachine.h"
#include "Inputs/System/InputSystem.h"
#include "Graphic/System/DeleterSystem.h"
#include "Graphic/System/DebugCollisionDrawerSystem.h"
#include "Draw/DrawableContainer.h"
#include "Physic/MarchingSquare.h"
#include "World/LayerLoader.h"
#include "Physic/Vect.h"
#include "Physic/Space.h"
#include "World/TileWorld.h"
#include "World/TileAgglomerate.h"

void DoThings() {
	using ska::cp::Vect;
	using ska::cp::Body;
	using ska::cp::Shape;
	using ska::cp::Space;

	const auto gravity = Vect{ 0, 3 };

	Space space;
	space.setGravity(gravity);

	auto& ground = space.addShape(Shape::fromSegment(space.getStaticBody(), Vect{ -20, 5 }, Vect{ 20, -5 }, 0));
	ground.setFriction(1);

	const auto radius = 5.F;
	const auto mass = 1.F;

	auto& ballBody = space.addBody(Body::fromRadius(mass, radius));
	ballBody.setPosition(Vect{ 0, 15 });

	auto& ballShape = space.addShape(Shape::fromCircle(ballBody.body(), radius, Vect{}));
	ballShape.setFriction(0.7F);

	const auto timeStep = 1.0 / 60.0;
	for (auto time = 0.; time < 20.; time += timeStep) {
		const auto pos = ballBody.getPosition();
		const auto vel = ballBody.getVelocity();
		printf(
			"Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n",
			time, pos.x, pos.y, vel.x, vel.y
		);

		space.step(timeStep);
	}
}

StateSandbox::StateSandbox(ska::EntityManager& em, ska::ExtensibleGameEventDispatcher<>& ed) :
	SubObserver<ska::GameEvent>(std::bind(&StateSandbox::onGameEvent, this, std::placeholders::_1), ed),
	SubObserver<ska::InputMouseEvent>(std::bind(&StateSandbox::onMouseEvent, this, std::placeholders::_1), ed),
	m_cameraSystem(nullptr),
	m_eventDispatcher(ed),
	m_entityManager(em){}

std::vector<ska::Rectangle> GenerateAgglomeratedTileMap(const ska::TileWorld& world) {
	auto physicCollisionMapX = ska::TileAgglomerate::apply(world, true);
	auto physicCollisionMapY = ska::TileAgglomerate::apply(world, false);

	std::unordered_set<ska::Point<int>> remainingBlocks;
	if (physicCollisionMapY.size() > physicCollisionMapX.size()) {
		return physicCollisionMapY;
	}

	return physicCollisionMapY;	
}

bool StateSandbox::onMouseEvent(ska::InputMouseEvent& ime){
	const auto& actions = ime.icm.getActions();
	if(actions[ska::LClic]) {
		const auto& ranges = ime.icm.getRanges();
		const auto mousePos = ranges[ska::MousePos];
		
		createBall(mousePos);
	}
	return true;
}

bool StateSandbox::onGameEvent(ska::GameEvent& ge) {
	
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY) {
		auto cameraSystemPtr = std::make_unique<ska::CameraFixedSystem>(m_entityManager, m_eventDispatcher, ge.windowWidth, ge.windowHeight, ska::Point<int>());
		m_cameraSystem = cameraSystemPtr.get();
		addLogic(std::move(cameraSystemPtr));
		addGraphic(std::make_unique<ska::GraphicSystem>(m_entityManager, m_eventDispatcher, m_cameraSystem));

		addLogic(std::make_unique<ska::DebugCollisionDrawerSystem>(m_entityManager));
		addLogic(std::make_unique<ska::DeleterSystem>(m_entityManager));
		addLogic(std::make_unique<ska::InputSystem>(m_entityManager, m_eventDispatcher));

		/*const ska::ChipsetCorrespondanceMapper corr{ "Resources/Chipsets/corr.png" };
		m_layerHolder.chipset = std::make_unique<ska::Chipset>( corr, 48, "Resources/Chipsets/chipset_platform" );
		ska::LayerLoader loader;

		auto layerData = loader.load("Resources/Levels/new_level/new_level.bmp", *m_layerHolder.chipset);

		const auto layerBlocks = std::move(layerData.physics);
		m_layerHolder.layerRenderableBlocks = std::move(layerData.graphics);*/

		ska::TileWorld world{ 48, "Resources/Chipsets/corr.png" };
		//TODO sortir completement le load de la classe world
		world.load("Resources/Levels/new_level", "Resources/Chipsets/chipset_platform");
		
		const auto agglomeratedTiles = GenerateAgglomeratedTileMap(world);
		
		m_space.setGravity({ 0., 0.0001 });
		m_ballTexture.loadFromColoredRect(10, 10, ska::Color{ 0, 125, 125, 255 });
		
	
		for (const auto& r : agglomeratedTiles) {
			m_space.addShape(ska::cp::Shape::fromBox(m_space.getStaticBody(), r, 1.));
		}

		m_layerContours.emplace_back(agglomeratedTiles);
		//DoThings();

	}
	return true;
}



void StateSandbox::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	drawables.addHead(m_layerHolder);
	for (auto& l : m_layerContours) {
		drawables.addHead(l);
	}
	for (const auto& ball : m_ballGraphics) {
		drawables.add(ball);
	}
}

void StateSandbox::onEventUpdate(unsigned int timeStep) {
	
	auto index = 0u;
	for (const auto& ball : m_space.getBodies()) {
		const auto pos = ball.getPosition();
		m_space.step(timeStep);
		m_ballGraphics[index].move(pos.x, pos.y);
		index++;
	}
	
}

void StateSandbox::createBall(const ska::Point<float>& point) {

	const auto ballBody = &m_space.addBody(ska::cp::Body::fromRadius(1.F, 5.F));
	ballBody->setPosition(ska::cp::Vect{ point.x, point.y });

	auto& sh = m_space.addShape(ska::cp::Shape::fromCircle(ballBody->body(), 5.F, ska::cp::Vect{}));
	sh.setFriction(1.F);
	
	m_ballGraphics.emplace_back(m_ballTexture, 0, 0, 1000, 1000);
}
