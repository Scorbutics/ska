#include <chipmunk.h>
#include "Space.h"
#include "Vect.h"
#include "Shape.h"

ska::cp::Space::Space() : 
	m_space(nullptr) {
	load();
}

ska::cp::Space::~Space() {
	free();
}

void ska::cp::Space::setGravity(const Vect& v) {
	cpSpaceSetGravity(m_space, v.vect());
}

ska::cp::Shape& ska::cp::Space::addShape(Shape&& shape) {
	m_shapes.push_back(std::move(shape));
	auto& ref = m_shapes.back();
	cpSpaceAddShape(m_space, ref.shape());
	return ref;
}

ska::cp::Body& ska::cp::Space::addBody(Body&& body) {
	m_bodies.emplace_back(std::move(body));
	auto& ref = m_bodies.back();
	cpSpaceAddBody(m_space, ref.body());
	return ref;
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
