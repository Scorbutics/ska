#include <algorithm>
#include <array>
#include <unordered_set>
#include <chipmunk/chipmunk.h>
#include "Space.h"
#include "Vect.h"
#include "Shape.h"
#include "../CollisionHandlerType.h"

#include "Logging/Logger.h"

std::pair<ska::EntityId*, ska::cp::SpaceUserData&> ska::cp::SpaceCollisionCallbackExtractUserData(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	auto& userData = *static_cast<ska::cp::SpaceUserData*>(userDataPtr);
	auto result = true;
	
	cpBody* a;
	cpBody* b;
	cpArbiterGetBodies(arb, &a, &b);

	const auto entityBody = cpSpaceGetStaticBody(space) == a ? b : a;
	auto bodyEntityId = static_cast<ska::EntityId*>(cpBodyGetUserData(entityBody));

	return std::pair<ska::EntityId*, ska::cp::SpaceUserData&>(bodyEntityId, userData);
}

template <ska::cp::CollisionHandlerType type>
bool CollisionCallbackCall(cpArbiter *arb, cpSpace *space, cpDataPointer userDataPtr) {
	auto result = true;
	auto [bodyEntityId, userData] = ska::cp::SpaceCollisionCallbackExtractUserData(arb, space, userDataPtr);
	auto arbiter = ska::cp::Arbiter{ *arb };

	if constexpr (type == ska::cp::CollisionHandlerType::PRE || type == ska::cp::CollisionHandlerType::BEGIN) {
		auto& callbacks = std::get<static_cast<int>(type)>(userData.defaultCallbacks);
		for (auto& callback : callbacks) {
			result &= callback.second(arbiter, *userData.space, bodyEntityId);
		}
		return result;
	} else {
		auto& callbacks = std::get<static_cast<int>(type)>(userData.defaultCallbacks);
		for (auto& callback : callbacks) {
			callback.second(arbiter, *userData.space, bodyEntityId);
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

ska::cp::Space::Space() {
	m_userData.space = this;
	m_space = nullptr;
	load();
	initCollisionHandlers();
}

ska::cp::Space::~Space() {
	free();
}

ska::cp::Space::Space(Space&& sh) noexcept {
	*this = std::move(sh);
}

void ska::cp::Space::setGravity(const Vect& v) {
	cpSpaceSetGravity(m_space, v.vect());
}

void ska::cp::Space::addCollisionHandler(unsigned int collisionTypeA, unsigned int collisionTypeB, CollisionHandlerData collisionHandlerData) {
	const auto col = cpSpaceAddCollisionHandler(m_space, collisionTypeA, collisionTypeB);
	col->userData = &m_userData;
	collisionHandlerData.setupCollisionHandler(*col);
}

void ska::cp::Space::addWildcardCollisionHandler(unsigned int collisionType, CollisionHandlerData collisionHandlerData) {
	const auto col = cpSpaceAddWildcardHandler(m_space, collisionType);
	col->userData = &m_userData;
	collisionHandlerData.setupCollisionHandler(*col);
}

void ska::cp::Space::addDefaultCollisionHandler(CollisionHandlerData collisionHandlerData) {
	const auto col = cpSpaceAddDefaultCollisionHandler(m_space);
	col->userData = &m_userData;
	collisionHandlerData.setupCollisionHandler(*col);
}

namespace {
	// Puts a value in a free place of the container, else at its end
	template <class T>
	std::size_t Put(std::vector<T>& container, std::vector<std::size_t>& free, T value) {
		if (free.empty()) {
			container.emplace_back(std::move(value));
			return container.size() - 1;
		}
		const auto index = free.back();
		free.pop_back();
		container[index] = std::move(value);
		return index;
	}

	bool IsFree(const std::vector<std::size_t>& free, std::size_t index) {
		return std::find(free.begin(), free.end(), index) != free.end();
	}
}

std::size_t ska::cp::Space::addConstraint(Body* linked, Body& linked2, Constraint c) {
	const auto index = Put(m_constraints, m_freeConstraints, std::move(c));
	cpSpaceAddConstraint(m_space, &m_constraints[index].constraint());
	if (linked != nullptr) {
		linked->linkConstraint(index);
	}
	linked2.linkConstraint(index);
	return index;
}

std::size_t ska::cp::Space::addShape(Body* linked, Shape shape) {
	const auto index = Put(m_shapes, m_freeShapes, std::move(shape));
	cpSpaceAddShape(m_space, &m_shapes[index].shape());
	if (linked != nullptr) {
		linked->linkShape(index);
	}
	return index;
}

std::size_t ska::cp::Space::addBody(Body body) {
	const auto index = Put(m_bodies, m_freeBodies, std::move(body));
	cpSpaceAddBody(m_space, &m_bodies[index].body());
	return index;
}

void ska::cp::Space::removeShape(const std::size_t index) {
	if (index >= m_shapes.size() || IsFree(m_freeShapes, index)) {
		return;
	}
	cpSpaceRemoveShape(m_space, &m_shapes[index].shape());
	m_shapes[index] = Shape{};
	m_freeShapes.push_back(index);
}

void ska::cp::Space::removeConstraint(const std::size_t index) {
	if (index >= m_constraints.size() || IsFree(m_freeConstraints, index)) {
		return;
	}
	cpSpaceRemoveConstraint(m_space, &m_constraints[index].constraint());
	m_constraints[index] = Constraint{};
	m_freeConstraints.push_back(index);
	// Its bodies forget it : its place is taken by another one
	for (auto& body : m_bodies) {
		body.unlinkConstraint(index);
	}
}

void ska::cp::Space::removeBody(const std::size_t index) {
	if (index >= m_bodies.size() || IsFree(m_freeBodies, index)) {
		return;
	}
	const auto constraints = m_bodies[index].getConstraints();
	const auto shapes = m_bodies[index].getShapes();
	for (const auto constraint : constraints) {
		removeConstraint(constraint);
	}
	for (const auto shape : shapes) {
		removeShape(shape);
	}
	cpSpaceRemoveBody(m_space, &m_bodies[index].body());
	m_bodies[index] = Body{};
	m_freeBodies.push_back(index);
}

void ska::cp::Space::eraseBodies(std::size_t firstIndex, std::size_t lastIndex) {
	const auto end = lastIndex == 0 ? m_bodies.size() : lastIndex;
	for (auto index = firstIndex; index < end; index++) {
		removeBody(index);
	}
}

void ska::cp::Space::clear() {
	eraseBodies(0);
}

ska::cp::Body& ska::cp::Space::getBody(std::size_t index) {
	return m_bodies[index];
}

ska::cp::Shape& ska::cp::Space::getShape(std::size_t index) {
	return m_shapes[index];
}

ska::cp::Constraint& ska::cp::Space::getConstaint(std::size_t index) {
	return m_constraints[index];
}

void ska::cp::Space::step(double timestep) {
	/*static auto accu = 0.;
	if (accu >= 1. && m_bodies.size() >= 2) {
		SKA_LOG_DEBUG("Body 0 pos : (", m_bodies[0].getPosition().x, "; ", m_bodies[0].getPosition().y, ")");
		SKA_LOG_DEBUG("Body 1 pos : (", m_bodies[1].getPosition().x, "; ", m_bodies[1].getPosition().y, ")", "\n");
		accu = 0.;
	}
	accu += timestep;*/
	cpSpaceStep(m_space, timestep);
}

void ska::cp::Space::setIterations(int iterations) {
	cpSpaceSetIterations(m_space, iterations);
}

void ska::cp::Space::setSleepTimeThreshold(float threshold) {
	cpSpaceSetSleepTimeThreshold(m_space, threshold);
}

cpBody& ska::cp::Space::getStaticBody() {
	return *cpSpaceGetStaticBody(m_space);
}

bool ska::cp::Space::staticShapeIn(const cpBB& box) const {
	auto found = false;
	cpSpaceBBQuery(m_space, box, CP_SHAPE_FILTER_ALL, [](cpShape* shape, void* data) {
		if (cpBodyGetType(cpShapeGetBody(shape)) == CP_BODY_TYPE_STATIC) {
			*static_cast<bool*>(data) = true;
		}
	}, &found);
	return found;
}

bool ska::cp::Space::shapeIn(const cpBB& box) const {
	auto found = false;
	cpSpaceBBQuery(m_space, box, CP_SHAPE_FILTER_ALL, [](cpShape*, void* data) {
		*static_cast<bool*>(data) = true;
	}, &found);
	return found;
}

cpBB ska::cp::Space::shapeBox(std::size_t shapeIndex) const {
	return cpShapeGetBB(&m_shapes[shapeIndex].shape());
}

void ska::cp::Space::load() {
	free();
	m_space = cpSpaceNew();
}

ska::cp::Space& ska::cp::Space::operator=(Space&& space) noexcept {
	m_userData.space = this;
	m_space = space.m_space;
	m_shapes = std::move(space.m_shapes);
	m_bodies = std::move(space.m_bodies);
	m_constraints = std::move(space.m_constraints);
	m_freeShapes = std::move(space.m_freeShapes);
	m_freeBodies = std::move(space.m_freeBodies);
	m_freeConstraints = std::move(space.m_freeConstraints);
	space.m_space = nullptr;
	initCollisionHandlers();
	return *this;
}

void ska::cp::Space::free() {
	cpSpaceFree(m_space);
	m_space = nullptr;
}
