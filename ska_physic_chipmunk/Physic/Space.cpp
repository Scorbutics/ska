#include <array>
#include <chipmunk.h>
#include "Space.h"
#include "Vect.h"
#include "Shape.h"
#include "../CollisionHandlerType.h"

template <ska::cp::CollisionHandlerType type>
bool CollisionCallbackCall(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	auto& userData = *static_cast<ska::cp::SpaceUserData*>(userDataPtr);
	auto result = true;

	cpBody* a;
	cpBody* b;
	cpArbiterGetBodies(arb, &a, &b);

	auto arbiter = ska::cp::Arbiter{ *arb };

	const auto entityBody = cpSpaceGetStaticBody(space) == a ? b : a;
	auto bodyEntityId = static_cast<ska::EntityId*>(cpBodyGetUserData(entityBody));

	if constexpr (type == ska::cp::CollisionHandlerType::PRE || type == ska::cp::CollisionHandlerType::BEGIN) {
		auto& callbacks = std::get<static_cast<int>(type)>(userData.callbacks);
		for (auto& callback : callbacks) {
			result &= callback.second(arbiter, userData.space, bodyEntityId);
		}
		return result;
	} else {
		auto& callbacks = std::get<static_cast<int>(type)>(userData.callbacks);
		for (auto& callback : callbacks) {
			callback.second(arbiter, userData.space, bodyEntityId);
		}
	}
	return true;
}

cpBool CollisionCallbackPre(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	return CollisionCallbackCall<ska::cp::CollisionHandlerType::PRE>(arb, space, userDataPtr);
}

cpBool CollisionCallbackBegin(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	return CollisionCallbackCall<ska::cp::CollisionHandlerType::BEGIN>(arb, space, userDataPtr);
}

void CollisionCallbackPost(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	CollisionCallbackCall<ska::cp::CollisionHandlerType::POST>(arb, space, userDataPtr);
}

void CollisionCallbackSeparate(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	CollisionCallbackCall<ska::cp::CollisionHandlerType::SEPARATE>(arb, space, userDataPtr);
}

void ska::cp::Space::initCollisionHandlers() {
	std::array<CollisionHandlerData, 4> chds = {
		ska::cp::CollisionHandlerTypeFunc<CollisionHandlerType::BEGIN>{CollisionCallbackBegin},
		ska::cp::CollisionHandlerTypeFunc<CollisionHandlerType::PRE>{CollisionCallbackPre},
		ska::cp::CollisionHandlerTypeFunc<CollisionHandlerType::POST>{CollisionCallbackPost},
		ska::cp::CollisionHandlerTypeFunc<CollisionHandlerType::SEPARATE>{CollisionCallbackSeparate}
	};

	for (auto& chd : chds) {
		addDefaultCollisionHandler(chd);
	}
}

ska::cp::Space::Space() :
	m_userData(*this),
	m_space(nullptr) {
	load();
	initCollisionHandlers();
}

ska::cp::Space::~Space() {
	free();
}

ska::cp::Space::Space(Space&& sh) noexcept :
	m_userData(*this) {
	m_space = sh.m_space;
	m_shapes = std::move(sh.m_shapes);
	m_bodies = std::move(sh.m_bodies);
	sh.m_space = nullptr;
	initCollisionHandlers();
}

void ska::cp::Space::setGravity(const Vect& v) {
	cpSpaceSetGravity(m_space, v.vect());
}

void ska::cp::Space::addCollisionHandler(unsigned int collisionTypeA, unsigned int collisionTypeB, CollisionHandlerData collisionHandlerData) {
	const auto col = cpSpaceAddCollisionHandler(m_space, collisionTypeA, collisionTypeB);
	col->userData = &m_userData;
	collisionHandlerData.setupCollisionHandler(*col);
}

void ska::cp::Space::addDefaultCollisionHandler(CollisionHandlerData collisionHandlerData) {
	const auto col = cpSpaceAddDefaultCollisionHandler(m_space);
	col->userData = &m_userData;
	collisionHandlerData.setupCollisionHandler(*col);
}

std::size_t ska::cp::Space::addConstraint(Constraint c) {
	m_constraints.emplace_back(std::move(c));
	cpSpaceAddConstraint(m_space, m_constraints.back().constraint());
	return m_constraints.size() - 1;
}

std::size_t ska::cp::Space::addShape(Shape shape) {
	m_shapes.emplace_back(std::move(shape));
	cpSpaceAddShape(m_space, m_shapes.back().shape());
	return m_shapes.size() - 1;
}

std::size_t ska::cp::Space::addBody(Body body) {
	m_bodies.emplace_back(std::move(body));
	cpSpaceAddBody(m_space, m_bodies.back().body());
	return m_bodies.size() - 1;
}

void ska::cp::Space::eraseShapes(std::size_t firstIndex, std::size_t lastIndex) {
	auto startShapesIt = m_shapes.begin() + firstIndex;
	auto endShapesIt = lastIndex == 0 ? m_shapes.end() : m_shapes.begin() + lastIndex;
	for (auto it = startShapesIt; it != endShapesIt; it++) {
		cpSpaceRemoveShape(m_space, it->shape());
	}
	m_shapes.erase(startShapesIt, endShapesIt);
}

void ska::cp::Space::eraseBodies(std::size_t firstIndex, std::size_t lastIndex) {
	auto startBodiesIt = m_bodies.begin() + firstIndex;
	auto endBodiesIt = lastIndex == 0 ? m_bodies.end() : m_bodies.begin() + lastIndex;
	for (auto it = startBodiesIt; it != endBodiesIt; it++) {
		cpSpaceRemoveBody(m_space, it->body());
	}

	m_bodies.erase(startBodiesIt, endBodiesIt);
}

void ska::cp::Space::clear() {
	eraseShapes(0);
	eraseBodies(0);
}

std::vector<ska::cp::Body>& ska::cp::Space::getBodies() {
	return m_bodies;
}

std::vector<ska::cp::Shape>& ska::cp::Space::getShapes() {
	return m_shapes;
}

std::vector<ska::cp::Constraint>& ska::cp::Space::getConstaints() {
	return m_constraints;
}

void ska::cp::Space::step(double timestep) {
	cpSpaceStep(m_space, timestep);
}

void ska::cp::Space::setIterations(int iterations) {
	cpSpaceSetIterations(m_space, iterations);
}

void ska::cp::Space::setSleepTimeThreshold(float threshold) {
	cpSpaceSetSleepTimeThreshold(m_space, threshold);
}

cpBody* ska::cp::Space::getStaticBody() {
	return cpSpaceGetStaticBody(m_space);
}

void ska::cp::Space::load() {
	free();
	m_space = cpSpaceNew();
}

void ska::cp::Space::free() {
	cpSpaceFree(m_space);
	m_space = nullptr;
}
