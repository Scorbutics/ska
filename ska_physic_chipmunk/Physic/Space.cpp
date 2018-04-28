#include <array>
#include <chipmunk.h>
#include "Space.h"
#include "Vect.h"
#include "Shape.h"
#include "../CollisionHandlerType.h"

cpBool CollisionCallbackPre(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	auto& userData = *static_cast<ska::cp::SpaceUserData*>(userDataPtr);
	auto result = true;
	auto& callbacks = std::get<static_cast<int>(ska::cp::CollisionHandlerType::PRE)>(userData.callbacks);
	for (auto& callback : callbacks) {
		result &= callback(*arb, userData.space);
	}
	return result;
}

cpBool CollisionCallbackBegin(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	auto& userData = *static_cast<ska::cp::SpaceUserData*>(userDataPtr);
	auto result = true;
	auto& callbacks = std::get<static_cast<int>(ska::cp::CollisionHandlerType::BEGIN)>(userData.callbacks);
	for (auto& callback : callbacks) {
		result &= callback(*arb, userData.space);
	}
	return result;
}

void CollisionCallbackPost(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	auto& userData = *static_cast<ska::cp::SpaceUserData*>(userDataPtr);
	auto& callbacks = std::get<static_cast<int>(ska::cp::CollisionHandlerType::POST)>(userData.callbacks);
	for (auto& callback : callbacks) {
		callback(*arb, userData.space);
	}
}

void CollisionCallbackSeparate(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	auto& userData = *static_cast<ska::cp::SpaceUserData*>(userDataPtr);
	auto& callbacks = std::get<static_cast<int>(ska::cp::CollisionHandlerType::SEPARATE)>(userData.callbacks);
	for (auto& callback : callbacks) {
		callback(*arb, userData.space);
	}
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

ska::cp::Constraint& ska::cp::Space::addConstraint(Constraint c) {
	m_constraints.emplace_back(std::move(c));
	auto& ref = m_constraints.back();
	cpSpaceAddConstraint(m_space, ref.constraint());
	return ref;
}

ska::cp::Shape& ska::cp::Space::addShape(Shape shape) {
	m_shapes.push_back(std::move(shape));
	auto& ref = m_shapes.back();
	cpSpaceAddShape(m_space, ref.shape());
	return ref;
}

ska::cp::Body& ska::cp::Space::addBody(Body body) {
	m_bodies.emplace_back(std::move(body));
	auto& ref = m_bodies.back();
	cpSpaceAddBody(m_space, ref.body());
	return ref;
}

std::vector<ska::cp::Body>& ska::cp::Space::getBodies() {
	return m_bodies;
}

std::vector<ska::cp::Shape>& ska::cp::Space::getShapes() {
	return m_shapes;
}

void ska::cp::Space::step(double timestep) {
	cpSpaceStep(m_space, timestep);
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
