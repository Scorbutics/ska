#include "StateSandbox.h"
#include "../ska_draw/Graphic/System/GraphicSystem.h"
#include "../ska_draw/Graphic/System/CameraFixedSystem.h"
#include "Physic/System/MovementSystem.h"
#include "Physic/System/CollisionSystem.h"
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/WalkAnimationStateMachine.h"
#include "Inputs/System/InputSystem.h"
#include "ECS/Basics/Physic/GravityAffectedComponent.h"
#include "Physic/System/GravitySystem.h"
#include "Draw/SDLRenderer.h"

constexpr const char* RESOURCES_FOLDER_RAW = "." FILE_SEPARATOR "Resources" FILE_SEPARATOR "Sprites" FILE_SEPARATOR;
#define RESOURCES_FOLDER std::string(RESOURCES_FOLDER_RAW)

StateSandbox::StateSandbox(StateData & data, ska::StateHolder & sh) :
	StateBase(data.m_entityManager, data.m_eventDispatcher, sh),
	SubObserver(std::bind(&StateSandbox::onGameEvent, this, std::placeholders::_1), data.m_eventDispatcher),
	m_cameraSystem(nullptr),
	m_eventDispatcher(data.m_eventDispatcher),
	m_entityManager(data.m_entityManager),
	m_entityCollision(data.m_eventDispatcher, data.m_entityManager), 
	m_walkASM(nullptr) {
	//TODO faire en sorte que l'ajout de système puisse se faire après la création d'entités
}

ska::EntityId StateSandbox::createPhysicAABBEntity(ska::Point<int> pos, const std::string& sprite, bool spritesheet) const {
	auto entity = m_entityManager.createEntity();
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
		fc.weight = 0.2;
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
	if (ge.getEventType() == ska::GAME_WINDOW_READY) {
		m_cameraSystem = addLogic<ska::CameraFixedSystem>(ge.windowWidth, ge.windowHeight, ska::Point<int>());
		addGraphic<ska::GraphicSystem>(m_eventDispatcher, m_cameraSystem);

		addLogic<ska::MovementSystem>();
		addLogic<ska::CollisionSystem>(m_eventDispatcher);
		addLogic<ska::GravitySystem>();
		addLogic<ska::InputSystem>(m_eventDispatcher);
        auto animSystem = addLogic<ska::AnimationSystem<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>>();
        m_walkASM = animSystem->setup<ska::WalkAnimationStateMachine>(m_entityManager).get();
		animSystem->setup<ska::JumpAnimationStateMachine>(m_entityManager);

		animSystem->link<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine>([&](ska::EntityId& e) {
			auto& mov = m_entityManager.getComponent<ska::MovementComponent>(e);
			return ska::NumberUtils::absolute(mov.vz) > 0.1;
		});

		animSystem->link<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>([&](ska::EntityId& e) {
			auto& mov = m_entityManager.getComponent<ska::MovementComponent>(e);
			return ska::NumberUtils::absolute(mov.vz) <= 0.1;
		});

		auto blockA = createPhysicAABBEntity(ska::Point<int>(100, 100), "1", true);
		auto blockB = createPhysicAABBEntity(ska::Point<int>(350, 150), "0", false);
		auto blockC = createPhysicAABBEntity(ska::Point<int>(200, 300), "0", false);



		auto& graphicComponentC = m_entityManager.getComponent<ska::GraphicComponent>(blockC);
		auto& asC = graphicComponentC.animatedSprites[0];
		asC.lifetimeSeparation();

		ska::SDLRenderer::getDefaultRenderer()->setRenderColor(ska::Color(70, 70, 0, 255));
		asC.setAlpha(70);
		asC.setBlendMode(SDL_BLENDMODE_ADD);

		asC.setColor(180, 255, 255);

		ska::AnimationComponent ac;
		ac.setASM(m_walkASM, blockA);
		m_entityManager.addComponent<ska::AnimationComponent>(blockA, std::move(ac));

		ska::InputComponent ic;
		ic.jumpPower = 2;
		ic.movePower = 0.2F;
		m_entityManager.addComponent<ska::InputComponent>(blockC, std::move(ic));

	} else if (ge.getEventType() == ska::GAME_WINDOW_RESIZED) {
		m_cameraSystem->screenResized(ge.windowWidth, ge.windowHeight);
	}
	return true;
}

void StateSandbox::onGraphicUpdate(unsigned int, ska::DrawableContainer&) {
}

void StateSandbox::onEventUpdate(unsigned int) {
}
