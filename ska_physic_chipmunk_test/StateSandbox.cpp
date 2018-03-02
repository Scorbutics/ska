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
#include "World/ChipsetCorrespondanceMapper.h"
#include "World/Chipset.h"
#include "Physic/Vect.h"
#include "Physic/Space.h"

constexpr const char* RESOURCES_FOLDER_RAW = "." FILE_SEPARATOR "Resources" FILE_SEPARATOR "Sprites" FILE_SEPARATOR;
#define RESOURCES_FOLDER std::string(RESOURCES_FOLDER_RAW)

StateSandbox::StateSandbox(ska::EntityManager& em, ska::ExtensibleGameEventDispatcher<>& ed) :
	SubObserver(std::bind(&StateSandbox::onGameEvent, this, std::placeholders::_1), ed),
	m_cameraSystem(nullptr),
	m_eventDispatcher(ed),
	m_entityManager(em) {
}

bool StateSandbox::onGameEvent(ska::GameEvent& ge) {
	using GameAnimationSystem = ska::AnimationSystem<ska::JumpAnimationStateMachine, ska::WalkAnimationStateMachine>;
	if (ge.getEventType() == ska::GAME_WINDOW_READY) {
		auto cameraSystemPtr = std::make_unique<ska::CameraFixedSystem>(m_entityManager, m_eventDispatcher, ge.windowWidth, ge.windowHeight, ska::Point<int>());
		m_cameraSystem = cameraSystemPtr.get();
		addLogic(std::move(cameraSystemPtr));
		addGraphic(std::make_unique<ska::GraphicSystem>(m_entityManager, m_eventDispatcher, m_cameraSystem));

		addLogic(std::make_unique<ska::DebugCollisionDrawerSystem>(m_entityManager));
		addLogic(std::make_unique<ska::DeleterSystem>(m_entityManager));
		addLogic(std::make_unique<ska::InputSystem>(m_entityManager, m_eventDispatcher));

		const ska::ChipsetCorrespondanceMapper corr{ "Resources/Chipsets/corr.png" };
		m_layerHolder.chipset = std::make_unique<ska::Chipset>( corr, 48, "Resources/Chipsets/chipset_platform" );
		ska::LayerLoader loader;

		auto layerData = loader.load("Resources/Levels/new_level/new_level.bmp", *m_layerHolder.chipset);

		auto layerBlocks = std::move(layerData.physics);
		m_layerHolder.layerRenderableBlocks = std::move(layerData.graphics);

		std::unordered_set<ska::Point<int>> remainingBlocks;


		ska::MarchingSquare ms;
		do {
			const auto pointList = ms.apply(layerBlocks, remainingBlocks);
			m_layerContours.push_back({ pointList, 48 });
		} while (!remainingBlocks.empty());

	}
	return true;
}

void DoThings() {
	using ska::cp::Vect;
	using ska::cp::Body;
	using ska::cp::Shape;
	using ska::cp::Space;

	const auto gravity = Vect{ 0, -100 };

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
	for (auto time = 0.; time < 2.; time += timeStep) {
		const auto pos = ballBody.getPosition();
		const auto vel = ballBody.getVelocity();
		printf(
			"Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n",
			time, pos.x, pos.y, vel.x, vel.y
		);

		space.step(timeStep);
	}
}

void StateSandbox::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	drawables.addHead(m_layerHolder);
	for (auto& l : m_layerContours) {
		drawables.addHead(l);
	}

}

void StateSandbox::onEventUpdate(unsigned int) {
}
