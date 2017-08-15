#include "StateSandbox.h"
#include "../ska_draw/Graphic/System/GraphicSystem.h"
#include "../ska_draw/Graphic/System/CameraFixedSystem.h"
#include "Physic/System/MovementSystem.h"
#include "Physic/System/CollisionSystem.h"
#include "Inputs/System/InputSystem.h"
#include "ECS/Basics/Physic/GravityAffectedComponent.h"
#include "Physic/System/GravitySystem.h"

constexpr const char* RESOURCES_FOLDER_RAW = "." FILE_SEPARATOR "Resources" FILE_SEPARATOR "Sprites" FILE_SEPARATOR;
#define RESOURCES_FOLDER std::string(RESOURCES_FOLDER_RAW)

StateSandbox::StateSandbox(StateData & data, ska::StateHolder & sh) :
	StateBase(data.m_entityManager, data.m_eventDispatcher, sh),
	SubObserver(std::bind(&StateSandbox::onGameEvent, this, std::placeholders::_1), data.m_eventDispatcher),
	m_cameraSystem(nullptr),
	m_eventDispatcher(data.m_eventDispatcher),
	m_entityManager(data.m_entityManager),
	m_entityCollision(data.m_eventDispatcher, data.m_entityManager){
	//TODO faire en sorte que l'ajout de système puisse se faire après la création d'entités
}

ska::EntityId StateSandbox::createPhysicAABBEntity(ska::Point<int> pos) const{
	auto entity = m_entityManager.createEntity();
	ska::GraphicComponent gc;
	gc.animatedSprites.resize(1);
	auto& at = gc.animatedSprites[0];
	//at.load(RESOURCES_FOLDER + "giphy.gif");
	at.load(RESOURCES_FOLDER + "0.png", 6, 8, 3);
	m_entityManager.addComponent<ska::GraphicComponent>(entity, std::move(gc));
	m_entityManager.addComponent<ska::GravityAffectedComponent>(entity, ska::GravityAffectedComponent());
	ska::ForceComponent fc;
	static int one = 0;
	if (one == 1) {
		fc.weight = std::numeric_limits<float>::max();
		fc.bounciness = 0;
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

		auto blockA = createPhysicAABBEntity(ska::Point<int>(100, 100));
		auto blockB = createPhysicAABBEntity(ska::Point<int>(350, 150));

		ska::InputComponent ic;
		m_entityManager.addComponent<ska::InputComponent>(blockA, std::move(ic));
	} else if (ge.getEventType() == ska::GAME_WINDOW_RESIZED) {
		m_cameraSystem->screenResized(ge.windowWidth, ge.windowHeight);
	}
	return true;
}

void StateSandbox::onGraphicUpdate(unsigned int, ska::DrawableContainer&) {
}

void StateSandbox::onEventUpdate(unsigned int) {
}
