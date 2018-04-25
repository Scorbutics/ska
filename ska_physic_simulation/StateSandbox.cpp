#include "StateSandbox.h"
#include "Graphic/System/GraphicSystem.h"
#include "Graphic/System/CameraFixedSystem.h"
#include "Physic/System/MovementSystem.h"
#include "Physic/System/CollisionSystem.h"
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/WalkAnimationStateMachine.h"
#include "Inputs/System/InputSystem.h"
#include "ECS/Basics/Physic/GravityAffectedComponent.h"
#include "Physic/System/GravitySystem.h"
#include "Graphic/System/DeleterSystem.h"
#include "Physic/System/WorldCollisionSystem.h"
#include "Graphic/System/DebugCollisionDrawerSystem.h"
#include "World/TileWorldLoaderAggregate.h"
#include "World/TilesetLoaderImage.h"
#include "World/TilesetEventLoaderText.h"
#include "World/TilesetCompleteLoader.h"
#include "World/LayerEventLoaderText.h"
#include "World/LayerLoaderImage.h"
#include "Utils/FileUtils.h"

constexpr const char* RESOURCES_FOLDER_RAW = "./Resources/Sprites/";
#define RESOURCES_FOLDER std::string(RESOURCES_FOLDER_RAW)

ska::Tileset BuildTileset() {
    const ska::TilesetCompleteLoader<ska::TilesetLoaderImage, ska::TilesetEventLoaderText> tilesetLoader { "Resources/Chipsets/chipset_platform" };
    return { 48, tilesetLoader.tilesetLoader, tilesetLoader.tilesetEventLoader };
}

ska::TileWorldLoaderAggregate BuildTileWorldLoaderImageAndText(const ska::TilesetCorrespondanceMapper& mapper, const std::string& levelName) {
    const auto levelFileName = ska::FileNameData {levelName};

    auto loaders = std::vector<std::unique_ptr<ska::LayerLoader>> {};
    loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + ".bmp"));
    loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + "M.bmp"));
    loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + "T.bmp"));

    auto eventLoaders = std::vector<std::unique_ptr<ska::LayerEventLoader>> {};
    eventLoaders.push_back(std::make_unique<ska::LayerEventLoaderText>(levelName + "/" + levelFileName.name + "E.txt"));

    return { levelName, std::move(loaders), std::move(eventLoaders) };
}

ska::TileWorld BuildTileWorld(const ska::TilesetCorrespondanceMapper& mapper, ska::ExtensibleGameEventDispatcher<>& ed, ska::Tileset& tileset) {
    const auto levelLoader = BuildTileWorldLoaderImageAndText(mapper, "Resources/Levels/new_level");
    return { ed, tileset, levelLoader };
}

StateSandbox::StateSandbox(ska::EntityManager& em, ska::ExtensibleGameEventDispatcher<>& ed) :
	SubObserver(std::bind(&StateSandbox::onGameEvent, this, std::placeholders::_1), ed),
	m_cameraSystem(nullptr),
	m_eventDispatcher(ed),
	m_entityManager(em),
	m_debugEntityCollision(ed, em),
	m_entityCollision(ed, em),
	m_debugWorldCollision(ed, em),
	m_mapper {"Resources/Chipsets/corr.png"},
	m_tileset {BuildTileset()},
	m_world (std::move(BuildTileWorld(m_mapper, ed, m_tileset))),
	m_worldCollisionResponse(m_world, ed, em),
	m_walkASM(nullptr) {
	//TODO faire en sorte que l'ajout de système puisse se faire après la création d'entités
}

ska::EntityId StateSandbox::createPhysicAABBEntity(ska::Point<int> pos, const std::string& sprite, bool spritesheet) const {
	const auto entity = m_entityManager.createEntity();
	ska::GraphicComponent gc;
	gc.animatedSprites.resize(1);
	auto& at = gc.animatedSprites[0];
	//at.load(RESOURCES_FOLDER + "giphy.gif");
	if (spritesheet) {
		at.load(RESOURCES_FOLDER + sprite + ".png", 4, 8, 3);
	} else {
		at.load(RESOURCES_FOLDER + sprite + ".png");
	}
	m_entityManager.addComponent<ska::GraphicComponent>(entity, std::move(gc));
	m_entityManager.addComponent<ska::GravityAffectedComponent>(entity, ska::GravityAffectedComponent());
	ska::ForceComponent fc;
	static auto one = 0;
	if (one == 1) {
		fc.weight = std::numeric_limits<float>::max();
		fc.bounciness = 0;
	} else {
		fc.weight = 0.2F;
	}
	one++;
	m_entityManager.addComponent<ska::ForceComponent>(entity, std::move(fc));
	ska::MovementComponent mc;
	m_entityManager.addComponent<ska::MovementComponent>(entity, std::move(mc));
	ska::PositionComponent pc;
	pc.x = pos.x;
	pc.y = pos.y;
	pc.z = 0;
	m_entityManager.addComponent<ska::PositionComponent>(entity, std::move(pc));

	m_entityManager.addComponent<ska::CollidableComponent>(entity, ska::CollidableComponent());
	ska::HitboxComponent hc;
	hc.height = at.getHeight();
	hc.width = at.getWidth();
	m_entityManager.addComponent<ska::HitboxComponent>(entity, std::move(hc));
	return entity;
}

bool StateSandbox::onGameEvent(ska::GameEvent& ge) {
	using GameAnimationSystem = ska::AnimationSystem<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>;
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY) {
		auto cameraSystemPtr = std::make_unique<ska::CameraFixedSystem>(m_entityManager, m_eventDispatcher, ge.windowWidth, ge.windowHeight, ska::Point<int>());
		m_cameraSystem = cameraSystemPtr.get();
		addLogic(std::move(cameraSystemPtr));
		addGraphic(std::make_unique<ska::GraphicSystem>(m_entityManager, m_eventDispatcher, m_cameraSystem));

		addLogic(std::make_unique<ska::MovementSystem>(m_entityManager));
		addLogic(std::make_unique<ska::CollisionSystem>(m_entityManager, m_eventDispatcher));
		addLogic(std::make_unique<ska::WorldCollisionSystem>(m_entityManager, m_world, m_eventDispatcher));
		addLogic(std::make_unique<ska::DebugCollisionDrawerSystem>(m_entityManager));
		addLogic(std::make_unique<ska::GravitySystem>(m_entityManager));
		addLogic(std::make_unique<ska::DeleterSystem>(m_entityManager));
		addLogic(std::make_unique<ska::InputSystem>(m_entityManager, m_eventDispatcher));
		auto animSystemPtr = std::make_unique<GameAnimationSystem>(m_entityManager);
		auto& animSystem = *animSystemPtr.get();
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

		auto blockA = createPhysicAABBEntity(ska::Point<int>(100, 100), "1", true);
		createPhysicAABBEntity(ska::Point<int>(350, 150), "0", false);
		auto blockC = createPhysicAABBEntity(ska::Point<int>(200, 300), "0", false);

		auto& graphicComponentC = m_entityManager.getComponent<ska::GraphicComponent>(blockC);
		auto& asC = graphicComponentC.animatedSprites[0];
		asC.lifetimeSeparation();

		ska::AnimationComponent ac;
		ac.setASM(*m_walkASM, blockA);
		m_entityManager.addComponent<ska::AnimationComponent>(blockA, std::move(ac));

		ska::InputComponent ic;
		ic.jumpPower = 2;
		ic.movePower = 0.2F;
		m_entityManager.addComponent<ska::InputComponent>(blockC, std::move(ic));

		//m_world.linkCamera(m_cameraSystem);
	}
	return true;
}

void StateSandbox::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	m_world.graphicUpdate(ellapsedTime, drawables);
}

void StateSandbox::onEventUpdate(unsigned int) {
}
