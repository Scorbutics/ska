#include <chipmunk.h>
#include "Space.h"
#include "Vect.h"
#include "Shape.h"
#include "../CollisionHandlerType.h"

ska::cp::Space::Space() : 
	m_space(nullptr) {
	load();
}

ska::cp::Space::~Space() {
	free();
}

ska::cp::Space::Space(Space&& sh) noexcept{
	m_space = sh.m_space;
	m_shapes = std::move(sh.m_shapes);
	m_bodies = std::move(sh.m_bodies);
	sh.m_space = nullptr;
}

ska::cp::Space& ska::cp::Space::operator=(Space&& sh) noexcept{
	std::swap(m_space, sh.m_space);
	std::swap(m_shapes, sh.m_shapes);
	std::swap(m_bodies, sh.m_bodies);
	return *this;
}

void ska::cp::Space::setGravity(const Vect& v) {
	cpSpaceSetGravity(m_space, v.vect());
}

void ska::cp::Space::addCollisionHandler(unsigned int collisionTypeA, unsigned int collisionTypeB, CollisionHandlerData collisionHandlerData) {
	const auto col = cpSpaceAddCollisionHandler(m_space, collisionTypeA, collisionTypeB);
	setCollisionCallbackForHandler(col, collisionHandlerData);
}

void ska::cp::Space::setCollisionCallbackForHandler(cpCollisionHandler* collisionHanlder, CollisionHandlerData& collisionHandlerData){
	switch(collisionHandlerData.getType()) {
	case CollisionHandlerType::BEGIN:
		collisionHanlder->beginFunc = collisionHandlerData.getHandler<CollisionHandlerType::BEGIN>();
		break;
	case CollisionHandlerType::PRE:
		collisionHanlder->preSolveFunc = collisionHandlerData.getHandler<CollisionHandlerType::PRE>();
		break;
	case CollisionHandlerType::POST:
		collisionHanlder->postSolveFunc = collisionHandlerData.getHandler<CollisionHandlerType::POST>();
		break;
	case CollisionHandlerType::SEPARATE:
		collisionHanlder->separateFunc = collisionHandlerData.getHandler<CollisionHandlerType::SEPARATE>();
		break;
	default:
		break;
	}
}

void ska::cp::Space::addDefaultCollisionHandler(CollisionHandlerData collisionCallback) {
	const auto col = cpSpaceAddDefaultCollisionHandler(m_space);
	setCollisionCallbackForHandler(col, collisionCallback);
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
