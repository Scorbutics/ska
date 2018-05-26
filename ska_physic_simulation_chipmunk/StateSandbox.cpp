#include "StateSandbox.h"
#include "Graphic/System/GraphicSystem.h"
#include "Graphic/System/CameraSystem.h"
#include "Inputs/System/InputSystem.h"
#include "Graphic/System/DeleterSystem.h"
#include "Graphic/System/DebugCollisionDrawerSystem.h"
#include "Draw/DrawableContainer.h"
#include "Physic/Space.h"
#include "World/TileWorld.h"
#include "Physic/MovementSystem.h"
#include "Physic/BuildHitbox.h"
#include "CollisionHandlerType.h"
#include "World/TileWorldLoaderAggregate.h"
#include "World/TilesetLoaderImage.h"
#include "World/TileWorldPhysics.h"
#include "World/TilesetEventLoaderText.h"
#include "World/TilesetCompleteLoader.h"
#include "World/LayerEventLoaderText.h"
#include "World/LayerLoaderImage.h"
#include "Utils/FileUtils.h"
#include "Graphic/System/CameraFixedStrategy.h"
#include "Graphic/System/WalkAnimationStateMachine.h"
#include "Graphic/System/AnimationSystem.h"
constexpr auto BLOCKSIZE = 48u;

StateSandbox::StateSandbox(ska::EntityManager& em, ska::ExtensibleGameEventDispatcher<>& ed) :
	SubObserver<ska::GameEvent>(std::bind(&StateSandbox::onGameEvent, this, std::placeholders::_1), ed),
	SubObserver<ska::InputMouseEvent>(std::bind(&StateSandbox::onMouseEvent, this, std::placeholders::_1), ed),
	m_eventDispatcher(ed),
	m_entityManager(em),
	m_spaceCollisionEventSender{ m_space, m_eventDispatcher, BLOCKSIZE } {
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

ska::TileWorldLoaderAggregate BuildTileWorldLoaderImageAndText(const ska::TilesetCorrespondanceMapper& mapper, const std::string& levelName) {
    const auto levelFileName = ska::FileNameData {levelName};

    auto loaders = std::vector<std::unique_ptr<ska::LayerLoader>> {};
    loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + ".bmp"));
    loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + "M.bmp"));
    loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + "T.bmp"));

    auto eventLoaders = std::vector<std::unique_ptr<ska::LayerEventLoader>> {};
    eventLoaders.push_back(std::make_unique<ska::LayerEventLoaderText>(levelName + "/" + levelFileName.name + "E.txt"));

    return { levelName, 1, std::move(loaders), std::move(eventLoaders) };
}

bool StateSandbox::onGameEvent(ska::GameEvent& ge) {

	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY) {
		auto cameraSystemPtr = std::make_unique<ska::CameraSystem>(m_entityManager, m_eventDispatcher, std::make_unique<ska::CameraFixedStrategy>(ska::Point<int>{}), ge.windowWidth, ge.windowHeight);
		m_cameraSystem = cameraSystemPtr.get();

		addLogic(std::move(cameraSystemPtr));
		addGraphic(std::make_unique<ska::GraphicSystem>(m_entityManager, m_eventDispatcher, *m_cameraSystem));

		addLogic(std::make_unique<ska::DebugCollisionDrawerSystem>(m_entityManager));
		addLogic(std::make_unique<ska::DeleterSystem>(m_entityManager));
		addLogic(std::make_unique<ska::InputSystem>(m_entityManager, m_eventDispatcher));
		addLogic(std::make_unique<ska::cp::MovementSystem>(m_entityManager, m_space));

		const ska::TilesetCompleteLoader<ska::TilesetLoaderImage, ska::TilesetEventLoaderText> tilesetLoader { "Resources/Chipsets/chipset_platform" };
		auto tileset = ska::Tileset{ BLOCKSIZE, tilesetLoader.tilesetLoader, tilesetLoader.tilesetEventLoader };
		const auto mapper = ska::TilesetCorrespondanceMapper {"Resources/Chipsets/corr.png"};
		const auto levelLoader = BuildTileWorldLoaderImageAndText(mapper, "Resources/Levels/new_level");

		const auto world = ska::TileWorld { m_eventDispatcher, tileset, levelLoader };

		const auto agglomeratedTiles = GenerateAgglomeratedTileMap(1, world.getCollisionProfile());
		const auto contourRectangleTile = GenerateContourTileMap(agglomeratedTiles);

		//m_space.setGravity({ 0., 300 });

		m_ballTexture.load("Resources/Sprites/2.png");

		for (const auto& r : contourRectangleTile) {
			m_space.addShape(ska::cp::Shape::fromBox(m_space.getStaticBody(), r));
		}

		m_layerContours.emplace_back(contourRectangleTile);

		using GameAnimationSystem = ska::AnimationSystem<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>;
		auto animSystemPtr = std::make_unique<GameAnimationSystem>(m_entityManager);
		auto& animSystem = *animSystemPtr;
		addLogic(std::move(animSystemPtr));
		m_walkASM = &animSystem.setup<ska::WalkAnimationStateMachine>(true, std::make_unique<ska::WalkAnimationStateMachine>(m_entityManager));
		animSystem.setup<ska::JumpAnimationStateMachine>(false, std::make_unique<ska::JumpAnimationStateMachine>(m_entityManager));

		animSystem.link<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine>([&](ska::EntityId& e) {
			auto& mov = m_entityManager.getComponent<ska::MovementComponent>(e);
			return ska::NumberUtils::absolute(mov.vz) > 0.1;
		});

		animSystem.link<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>([&](ska::EntityId& e) {
			auto& mov = m_entityManager.getComponent<ska::MovementComponent>(e);
			return ska::NumberUtils::absolute(mov.vz) <= 0.1;
		});
		
	}
	return true;
}

void StateSandbox::onGraphicUpdate(unsigned int, ska::DrawableContainer& drawables) {
	//drawables.addHead(m_layerHolder);
	for (auto& l : m_layerContours) {
		drawables.addHead(l);
	}
	for (const auto& ball : m_ballGraphics) {
		drawables.add(ball);
	}
}

void StateSandbox::onEventUpdate(const unsigned int timeStep) {
	m_space.step(timeStep/1000.);
}

void StateSandbox::createBall(const ska::Point<float>& point) {
	auto ballEntity = m_entityManager.createEntity();

	auto pc = ska::PositionComponent{};
	pc.layer = 1;
	pc.x = point.x;
	pc.y = point.y;
	m_entityManager.addComponent(ballEntity, std::move(pc));

	m_entityManager.addComponent(ballEntity, ska::MovementComponent{});

	ska::GraphicComponent gc;
	gc.sprites.push_back(m_ballTexture);
	m_entityManager.addComponent(ballEntity, std::move(gc));

	ska::ForceComponent fc;
	m_entityManager.addComponent(ballEntity, std::move(fc));

	ska::InputComponent ic;
	ic.movePower = 10.;

	auto hc = ska::HitboxComponent{};
	hc.width = 16;
	hc.height = 16;
	m_entityManager.addComponent(ballEntity, std::move(hc));

	m_entityManager.addComponent(ballEntity, std::move(ic));

	auto bc = ska::cp::BuildRectangleHitbox(m_space, { static_cast<int>(point.x), static_cast<int>(point.y), 16, 16 }, 200.f, 500.f, ballEntity);
	m_entityManager.addComponent(ballEntity, std::move(bc));

	m_balls.emplace_back(ballEntity);
}
