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
	ska::AnimatedTexture at;
	at.load(RESOURCES_FOLDER + "0.png", 1, 1, 1);
	gc.sprite.push_back(at);
	m_entityManager.addComponent<ska::GraphicComponent>(entity, gc);
	m_entityManager.addComponent<ska::GravityAffectedComponent>(entity, ska::GravityAffectedComponent());
	ska::ForceComponent fc;
	static int one = 0;
	if (one == 0) {
		fc.weight = std::numeric_limits<float>::max();
	}
	one++;
	m_entityManager.addComponent<ska::ForceComponent>(entity, fc);
	ska::MovementComponent mc;
	m_entityManager.addComponent<ska::MovementComponent>(entity, mc);
	ska::PositionComponent pc;
	pc.x = pos.x;
	pc.y = pos.y;
	pc.z = 0;
	m_entityManager.addComponent<ska::PositionComponent>(entity, pc);

	m_entityManager.addComponent<ska::CollidableComponent>(entity, ska::CollidableComponent());
	ska::HitboxComponent hc;
	hc.height = at.getHeight();
	hc.width = at.getWidth();
	m_entityManager.addComponent<ska::HitboxComponent>(entity, hc);
	return entity;
}

bool StateSandbox::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GAME_WINDOW_READY) {
		m_cameraSystem = addLogic<ska::CameraFixedSystem>(ge.windowWidth, ge.windowHeight, ska::Point<int>());
		addGraphic<ska::GraphicSystem>(m_eventDispatcher, m_cameraSystem);

		addLogic<ska::MovementSystem>();
		addLogic<ska::CollisionSystem>(m_world, m_eventDispatcher);
		addLogic<ska::GravitySystem>();
		addLogic<ska::InputSystem>(m_eventDispatcher);

		auto blockA = createPhysicAABBEntity(ska::Point<int>(100, 100));
		auto blockB = createPhysicAABBEntity(ska::Point<int>(350, 150));

		ska::InputComponent ic;
		ic.movePower = std::numeric_limits<float>::max()*0.5F;
		m_entityManager.addComponent<ska::InputComponent>(blockA, ic);

		//m_entityManager.addComponent<ska::InputComponent>(blockB, ska::InputComponent());

		auto& gac = m_entityManager.getComponent<ska::GravityAffectedComponent>(blockA);
		gac.friction = std::numeric_limits<float>::max()*0.03F;
		
	} else if (ge.getEventType() == ska::GAME_WINDOW_RESIZED) {
		m_cameraSystem->screenResized(ge.windowWidth, ge.windowHeight);
	}
	return true;
}

void StateSandbox::onGraphicUpdate(unsigned int, ska::DrawableContainer&) {
}

void StateSandbox::onEventUpdate(unsigned int) {
}
