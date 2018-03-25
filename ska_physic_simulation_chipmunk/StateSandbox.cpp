#include "StateSandbox.h"
#include "Graphic/System/GraphicSystem.h"
#include "Graphic/System/CameraFixedSystem.h"
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/WalkAnimationStateMachine.h"
#include "Inputs/System/InputSystem.h"
#include "Graphic/System/DeleterSystem.h"
#include "Graphic/System/DebugCollisionDrawerSystem.h"
#include "Draw/DrawableContainer.h"
#include "World/LayerLoader.h"
#include "Physic/Vect.h"
#include "Physic/Space.h"
#include "World/TileWorld.h"
#include "Physic/MovementSystem.h"
#include "Physic/BuildHitbox.h"
#include "CollisionHandlerType.h"
#include "World/MarchingSquare.h"

StateSandbox::StateSandbox(ska::EntityManager& em, ska::ExtensibleGameEventDispatcher<>& ed) :
	SubObserver<ska::GameEvent>(std::bind(&StateSandbox::onGameEvent, this, std::placeholders::_1), ed),
	SubObserver<ska::InputMouseEvent>(std::bind(&StateSandbox::onMouseEvent, this, std::placeholders::_1), ed),
	m_cameraSystem(nullptr),
	m_eventDispatcher(ed),
	m_entityManager(em){}

std::list<ska::Point<int>> GenerateAgglomeratedTileMap(const ska::TileWorld& world) {
	/*auto physicCollisionMapX = ska::TileAgglomerate::apply(world, true);
	auto physicCollisionMapY = ska::TileAgglomerate::apply(world, false);

	if (physicCollisionMapY.size() > physicCollisionMapX.size()) {
		return physicCollisionMapY;
	}*/

	std::list<ska::Point<int>> result;
	std::unordered_set<ska::Point<int>> remainingBlocks;
	std::list<ska::Point<int>> pointList;
	bool done = false;
	do {
		std::tie(done, pointList) = ska::MarchingSquare(world, remainingBlocks, [](const ska::Block* b) {
			return b != nullptr ? b->getCollision() : ska::BlockCollision::NO;
		});
		for(const auto& p : pointList) {
			result.push_back(p);
		}
	} while (!done);

	
	return result;
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

cpBool CollisionCallbackPre(cpArbiter *arb, cpSpace *space, cpDataPointer userData) {
	std::cout << "collisionBegin" << std::endl;
	const auto firstPointA = cpArbiterGetPointA(arb, 0);
	std::cout << "Point x : "<< firstPointA.x << "; y : " << firstPointA.y << std::endl;
	return true;
}

ska::Vector2<ska::Rectangle> GenerateContourTileMap(const std::list<ska::Point<int>>& list) {
	ska::Vector2<ska::Rectangle> contours;
	
	auto lastPointIt = list.cbegin();
	auto pIt = list.cbegin();

	if (pIt != list.cend()) {
		++pIt;
		for (; pIt != list.cend(); ++pIt) {
			auto rect = ska::RectangleUtils::createRectangleFromPoints((*lastPointIt) * 48, (*pIt)  * 48);
			if(rect.w == 0) {
				rect.w = 1;
				rect.x--;
			}
			if(rect.h == 0) {
				rect.h = 1;
				rect.y--;
			}
			contours.push_back(std::move(rect));
			lastPointIt = pIt;
		}
	}	
	return contours;
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
		const auto contourRectangleTile = GenerateContourTileMap(agglomeratedTiles);

		//m_space.setGravity({ 0., 300 });

		m_ballTexture.load("Resources/Sprites/2.png");

		for (const auto& r : contourRectangleTile) {
			m_space.addShape(ska::cp::Shape::fromBox(m_space.getStaticBody(), r));
		}

		m_layerContours.emplace_back(agglomeratedTiles);
		
		auto chd = ska::cp::CollisionHandlerData{
			ska::cp::CollisionHandlerTypeFunc<ska::cp::CollisionHandlerType::PRE>{CollisionCallbackPre}};
		
		m_space.addDefaultCollisionHandler(std::move(chd));

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
	auto ballEntity = m_entityManager.createEntity();

	m_entityManager.addComponent(ballEntity, ska::PositionComponent{});
	
	ska::GraphicComponent gc;
	gc.sprites.push_back(m_ballTexture);
	m_entityManager.addComponent(ballEntity, std::move(gc));

	ska::ForceComponent fc;
	m_entityManager.addComponent(ballEntity, std::move(fc));

	ska::InputComponent ic;
	ic.movePower = 10.;
	
	m_entityManager.addComponent(ballEntity, std::move(ic));
	
	auto bc = ska::cp::BuildCircleHitbox(m_space, point, 200.f, 500.f);
	m_entityManager.addComponent(ballEntity, std::move(bc));

	m_balls.emplace_back(ballEntity);
}
