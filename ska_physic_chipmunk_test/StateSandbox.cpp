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
#include "Physic/MovementSystem.h"

StateSandbox::StateSandbox(ska::EntityManager& em, ska::ExtensibleGameEventDispatcher<>& ed) :
	SubObserver<ska::GameEvent>(std::bind(&StateSandbox::onGameEvent, this, std::placeholders::_1), ed),
	SubObserver<ska::InputMouseEvent>(std::bind(&StateSandbox::onMouseEvent, this, std::placeholders::_1), ed),
	m_cameraSystem(nullptr),
	m_eventDispatcher(ed),
	m_entityManager(em){}

std::vector<ska::Rectangle> GenerateAgglomeratedTileMap(const ska::TileWorld& world) {
	/*auto physicCollisionMapX = ska::TileAgglomerate::apply(world, true);
	auto physicCollisionMapY = ska::TileAgglomerate::apply(world, false);

	if (physicCollisionMapY.size() > physicCollisionMapX.size()) {
		return physicCollisionMapY;
	}*/

	return ska::TileAgglomerate::apply(world);	
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
		addLogic(std::make_unique<ska::cp::MovementSystem>(m_entityManager, m_space));
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
		
		m_space.setGravity({ 0., 1 });

		m_ballTexture.loadFromColoredRect(10, 10, ska::Color{ 0, 125, 125, 255 });

		for (const auto& r : agglomeratedTiles) {
			m_space.addShape(ska::cp::Shape::fromBox(m_space.getStaticBody(), r, 0.));
		}

		m_layerContours.emplace_back(agglomeratedTiles);
		//DoThings();

	}
	return true;
}



void StateSandbox::onGraphicUpdate(unsigned int, ska::DrawableContainer& drawables) {
	drawables.addHead(m_layerHolder);
	for (auto& l : m_layerContours) {
		drawables.addHead(l);
	}
	for (const auto& ball : m_ballGraphics) {
		drawables.add(ball);
	}
}

void StateSandbox::onEventUpdate(unsigned int timeStep) {
	m_space.step(timeStep/1000.);
}

void StateSandbox::createBall(const ska::Point<float>& point) {

	const auto ballBody = &m_space.addBody(ska::cp::Body::fromMoment(1., INFINITY));
	const auto ballBodyIndex = m_space.getBodies().size() - 1;
	ballBody->setPosition(ska::cp::Vect{ point.x, point.y });

	auto& ballShape = m_space.addShape(ska::cp::Shape::fromCircle(ballBody->body(), 5., ska::cp::Vect{}));
	ballShape.setFriction(500.F);
	
	auto ballEntity = m_entityManager.createEntity();
	m_entityManager.addComponent(ballEntity, ska::PositionComponent{});
	
	ska::GraphicComponent gc;
	gc.sprites.push_back(m_ballTexture);
	m_entityManager.addComponent(ballEntity, std::move(gc));

	ska::ForceComponent fc;
	m_entityManager.addComponent(ballEntity, std::move(fc));

	ska::InputComponent ic;
	ic.movePower = 1.;
	
	m_entityManager.addComponent(ballEntity, std::move(ic));
	
	ska::cp::BodyComponent bc{};
	bc.bodyIndex = ballBodyIndex;

	m_entityManager.addComponent(ballEntity, std::move(bc));

	m_balls.emplace_back(ballEntity);
}
